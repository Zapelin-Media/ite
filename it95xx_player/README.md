Zapelin Media Injector
===

is a transport stream injector for ITE it95xx DVB/ISDB usb adapters.


INSTALLING
----------

      git clone https://github.com/zapelin-media/zmi.git
      cd zmi
      make && make install


USAGE
-----

      zmi [card] [mode] [frequency] [bandwidth] [constellation] [code rate] [transmission mode] [gain] [timeout]

where:

__card__: usb card number starting from 0

__mode__: 
- 0  DVB EAGLEI
- 1  DVB EAGLEII
- 2  ISDB EAGLEII
      
__frequency__: allowed frequency in Khz

__bandwidth__: allowed bandwidth in Khz

__constellation__:  
- 0  QPSK
- 1  16QAM
- 2  64QAM

__code rate__:  
- 0  1/2
- 1  2/3
- 2  3/4
- 3  5/6
- 4  7/8
                
__transmission mode__:  
- 0  2K
- 1  8K
- 2  4K
                    
__gain__: from -25 to +6

__timeout__:  number of seconds to wait without incoming data before ending the program

