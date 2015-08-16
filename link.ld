/*ENTRY(entry)
用参数*/
SECTIONS
{
    .text : AT(0x9000)
    {
        _code = .;
        *(.text)
        *(.rodata*)
    }
   .data : AT(ADDR(.text)+SIZEOF(.text))
   {
        _data = .;
        *(.data)
   }
   .bss : AT(ADDR(.data)+SIZEOF(.data))
   {
       _bss = .;
       *(.bss)
       *(COMMON)
   }
   .eh_frame : AT(ADDR(.bss)+SIZEOF(.bss))
   {
       _ehframe = .;
       *(.eh_frame)
   }
   _end = .;
   /DISCARD/ :
   {
        *(.comment)
   }
}