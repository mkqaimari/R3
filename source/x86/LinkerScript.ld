OUTPUT_FORMAT("binary")
ENTRY(start)

SECTIONS
{
    . = 0x00100000;
    code = 0x00100000 ;
    mboot = code + 0x0C  ;

  .text  : {
    *(.text)
    . = ALIGN(4096);
  }


  .rodata  :  
  {
    *(.rodata)
    . = ALIGN(4096);

  }

  data = ALIGN(4096) ;

  .data  : 
  {
    *(.data)
    . = ALIGN(4096);
  }

  bss = ALIGN(4096) ;

  .bss  : 
  {
    *(.bss); 
  }

  end = . ;

}
