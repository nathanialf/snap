/*
 * pokemonsnap.us.ld — placeholder linker script
 *
 * splat regenerates this file when 'segments:' is populated in
 * config/snap.us.yaml. The committed copy is a stub so the build does
 * not error out before any segments have been declared. Do NOT add
 * memory-map addresses by hand here — drive them from splat config so
 * the truth lives in one place.
 */

ENTRY(_start)

SECTIONS
{
    /* segments will be inserted by splat */

    /DISCARD/ : {
        *(.MIPS.abiflags)
        *(.reginfo)
        *(.note.*)
        *(.comment)
    }
}
