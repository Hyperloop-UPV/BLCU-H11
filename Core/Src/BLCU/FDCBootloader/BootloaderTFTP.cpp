
#include "BLCU/FDCBootloader/BootloaderTFTP.hpp"
#include "BLCU/BLCU.hpp"

bool BTFTP::error_ok = true;
bool BTFTP::ready = false;

BTFTP::Mode BTFTP::mode = BTFTP::Mode::NONE;

__attribute__((section(".ram_d1.buffer")))
BTFTP::btftp_file_t BTFTP::file = {};

//Public:
void BTFTP::on(BTFTP::Mode mode){
	BTFTP::ready = true;
	BTFTP::mode = mode;
}

void BTFTP::off(){
	BTFTP::ready = false;
	BTFTP::mode = BTFTP::Mode::NONE;
}

void BTFTP::start(){
	const tftp_context* context = new tftp_context(&BTFTP::open, &BTFTP::close, &BTFTP::read, &BTFTP::write);
	err_t error = tftp_init(context);

	if (error != ERR_OK) {
		ErrorHandler("Unable to start TFTP server, error code: %lu.", error);
		
		return;
	}
	memset(BTFTP::file.payload, 0, SECTOR_SIZE_IN_BYTES);
	BTFTP::on(BTFTP::Mode::WRITE);

}

//Private:
void* BTFTP::open(const char* fname, const char* mode, u8_t write){
	error_ok = true;
	if (not BTFTP::ready || write != (uint8_t)BTFTP::mode) {
		BLCU::finish_write_read_order(false);
		return nullptr;
	}

	const char* accepted_mode = "octet";
	if (strcmp(mode, accepted_mode)) {
		BLCU::finish_write_read_order(false);
		return nullptr;
	}

	uint8_t version = 0x0;

	if (not FDCB::get_version(version)){
		ErrorHandler("Bootloader not respondig, unable to start read/write operation!");
		BLCU::finish_write_read_order(false);
		return nullptr;
	}

	if (version != FDCB_CURRENT_VERSION) {
		ErrorHandler("Mismatch in bootloader version, current version in host: 0x%X in target: 0x%X.", FDCB_CURRENT_VERSION, version);
		BLCU::finish_write_read_order(false);
		return nullptr;
	}

	uint32_t address = FLASH_SECTOR0_START_ADDRESS;
	BTFTP::BHandle* handle = new BTFTP::BHandle(string(fname), string(mode), write, address);
	handle->file = &BTFTP::file;
	handle->current_sector = 0;
	BTFTP::file.max_pointer = SECTOR_SIZE_IN_BYTES - 1;

	if (handle->read_write == 1) {
		handle->file->pointer = 0;
	}else{
		handle->file->pointer = handle->file->max_pointer;
	}

	return handle;
}

void BTFTP::close(void* handle){
	memset(BTFTP::file.payload, 0, SECTOR_SIZE_IN_BYTES);
	BTFTP::file.pointer = 0;
	BTFTP::file.max_pointer = SECTOR_SIZE_IN_BYTES - 1;
	delete static_cast<BTFTP::BHandle*>(handle);
	BLCU::finish_write_read_order(error_ok);
}

int BTFTP::read(void* handle, void* buf, int bytes){
	BTFTP::BHandle* btftp_handle = (BTFTP::BHandle*)handle;
	if (btftp_handle->read_write == 1) {
		error_ok = false;
		return -1;
	}

	if (btftp_handle->file->pointer >= btftp_handle->file->max_pointer) {

		if (btftp_handle->current_sector > 5) {
			return 0;
		}else{
			if (not FDCB::read_memory(btftp_handle->current_sector, btftp_handle->file->payload)) {
				error_ok = false;
				return -1;
			}
			btftp_handle->file->pointer = 0;
			btftp_handle->current_sector++;
		}
	}

	memcpy((uint8_t*)buf, &btftp_handle->file->payload[btftp_handle->file->pointer], 512);
	btftp_handle->file->pointer += TFTP_MAX_DATA_SIZE;

	return 512;
}

int BTFTP::write(void* handle, struct pbuf* p){
	BTFTP::BHandle* btftp_handle = (BTFTP::BHandle*)handle;
	if (btftp_handle->read_write == 0) {
		error_ok = false;
		return -1;
	}



	if (btftp_handle->file->pointer >= btftp_handle->file->max_pointer) {
		if (btftp_handle->current_sector > 5) {
			return 1;
		}else{
			if (not FDCB::write_memory(btftp_handle->current_sector, btftp_handle->file->payload,(btftp_handle->file->pointer))) {
				error_ok = false;
				return -1;
			}
			btftp_handle->file->pointer = 0;
			btftp_handle->current_sector++;
		}
	}

	memcpy(&btftp_handle->file->payload[btftp_handle->file->pointer], (uint8_t*)p->payload, p->len);

	if (p->len < TFTP_MAX_DATA_SIZE) {
		uint32_t add_amount = 64-(p->len%64);
		btftp_handle->file->pointer += (p->len + add_amount);
		for(uint32_t i = p->len; i < add_amount; i++){
			btftp_handle->file->payload[btftp_handle->file->pointer] = 255;
			btftp_handle->file->pointer++;
		}
		if (not FDCB::write_memory(btftp_handle->current_sector, btftp_handle->file->payload,btftp_handle->file->pointer)) {
			error_ok = false;
			return -1;
		}
		return 0;
	}else{
		btftp_handle->file->pointer += TFTP_MAX_DATA_SIZE;
	}

	return 1;
}



