#include <linuxice.h>
#include <linux/kallsyms.h>
#include <linux/fb.h>

PFN_KALLSYMS_LOOKUP_NAME fn_kallsyms_lookup_name = kallsyms_lookup_name;
PFN_KALLSYMS_LOOKUP fn_kallsyms_lookup = NULL;
PFN_SPRINT_SYMBOL   fn_sprint_symbol = NULL;

void kallsyms_setup(void) {
    fn_kallsyms_lookup = (PFN_KALLSYMS_LOOKUP)kallsyms_lookup_name("kallsyms_lookup");
    fn_sprint_symbol = (PFN_SPRINT_SYMBOL)kallsyms_lookup_name("sprint_symbol");
}
