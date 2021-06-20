/*
 * drivers/misc/extender.c
 *
 * Address Span Extender
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include <asm-arm/mmio.h>
/*#include <xen/domain_page.h>
#include <xen/mm.h>
#include <xen/vmap.h>
#include <asm/io.h>*/

#include <xen/sched.h>

static enum io_state extender_map(struct vcpu *v, mmio_info_t *info)
{
    vaddr_t gva =  READ_SYSREG(FAR_EL2);

    /*TODO: fix printk format, probably phys addr have a specifier */
    printk("%s(): handle data abort at IPA->PA: %#"PRIpaddr"->%#"PRIpaddr" (VA:%#"PRIvaddr")\n",
            __func__, info->gpa, info->gpa, gva);

    map_mmio_regions(v->domain,
                     gaddr_to_gfn(info->gpa),
                     info->dabt.size / PAGE_SIZE,
                     maddr_to_mfn(info->gpa));

    return IO_HANDLED;
}

static int extender_mmio_read(struct vcpu *v, mmio_info_t *info,
                              register_t *r, void *priv)
{
    return extender_map(v, info);
}

static int extender_mmio_write(struct vcpu *v, mmio_info_t *info,
                              register_t r, void *priv)
{
    return extender_map(v, info);
}

static const struct mmio_handler_ops extender_mmio_handler = {
    .read  = extender_mmio_read,
    .write = extender_mmio_write,
};

int domain_extender_init(struct domain *d)
{
    int rc = 0;
    register_mmio_handler(d, &extender_mmio_handler,
                          0x00000090003000, PAGE_SIZE, NULL);

    return rc;
}

void domain_extender_deinit(struct domain *d)
{
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
