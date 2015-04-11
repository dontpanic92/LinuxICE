#include <linuxice.h>
#include <distorm3/distorm.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

void my_handler(void);
void fake_stub(void);

#define CODE_INFO(offset, size) {.code = (void*)(offset), .dt = Decode64Bits, .codeOffset = (unsigned long)offset, .codeLen = (unsigned long)(size), .features = DF_NONE}

void get_symbol(unsigned long addr, char* buf) {
    char nbuf[KSYM_NAME_LEN + 1];
    fn_sprint_symbol(nbuf, addr);
    strncpy(buf, nbuf, 47);
}

ice_disasm_attr ice_new_disasm(void* offset, unsigned int size) {
    ice_disasm_attr attr = {.inst = NULL, .count = 0, .offset = (unsigned long)offset, .size = size};
    _DecodeResult res;
    _DInst *tmp;
    _CodeInfo ci = CODE_INFO(offset, size);

    tmp = kmalloc(size * sizeof(_DInst), GFP_ATOMIC);

    res = distorm_decompose(&ci, tmp, size, &attr.count);

    attr.inst = kmalloc(attr.count * sizeof(_DInst), GFP_ATOMIC);
    memcpy(attr.inst, tmp, attr.count * sizeof(_DInst));
    kfree(tmp);

    tmp = NULL;

    return attr;
}

unsigned long ice_disasm_format(const ice_disasm_attr* attr, char* buf, unsigned int buf_size) {
    _DecodedInst di;
    _CodeInfo ci = CODE_INFO(attr->offset, attr->size);
    unsigned int cur_size = 0, i = 0, tmpret = 0;
    char namebuf[KSYM_NAME_LEN + 1], tmp[2];

    for(i = 0; i < attr->count; i++) {
        if(cur_size >= buf_size)
            break;
        linuxice_format(&ci, &((_DInst*)attr->inst)[i], &di, get_symbol);
        fn_sprint_symbol(namebuf, ((_DInst*)attr->inst)[i].addr);

        //tmpret = snprintf(tmp, 0, "%0*llX %-25s (%02d) %-24s %s%s%s\n", ci.dt != Decode64Bits ? 8 : 16, di.offset, namebuf, di.size, (char*)di.instructionHex.p, (char*)di.mnemonic.p, di.operands.length != 0 ? " " : "", (char*)di.operands.p);
        /////tmpret = snprintf(tmp, 0, "%-25s %-12s %s%s%s\n", namebuf, (char*)di.instructionHex.p, (char*)di.mnemonic.p, di.operands.length != 0 ? " " : "", (char*)di.operands.p);
        //if(cur_size + tmpret > buf_size - 1){
        //return;
        //}
        
        //snprintf(buf, buf_size - cur_size, "%0*llX %-25s (%02d) %-24s %s%s%s\n",
        tmpret = snprintf(buf, buf_size - cur_size, "%-25s %-12s %s%s%s\n",
                 //ci.dt != Decode64Bits ? 8 : 16,
                 //di.offset,
                 namebuf,
                 //di.size,
                 (char*)di.instructionHex.p,
                 (char*)di.mnemonic.p,
                 di.operands.length != 0 ? " " : "",
                 (char*)di.operands.p);

        cur_size += tmpret;
        buf += tmpret;
    }

    return cur_size;
}

void ice_free_disasm(ice_disasm_attr* attr) {
    kfree(attr->inst);
    attr->inst = NULL;
}
