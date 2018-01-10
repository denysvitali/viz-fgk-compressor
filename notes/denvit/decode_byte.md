# Decode Byte

## Decoder input
`(I) 0( ) 00(s) 100(e) 000(x) 100(u)`

| Character  | HEX  | Binary |
|------------|------| ------ |
| I | 0x49 | 0100 1001 |
|   | 0x20 | 0010 0000  |
| s | 0x73 | 0111 0011 |
| e | 0x65 | 0110 0101 |
| x | 0x78 | 0111 1000 | 
| u | 0x75 | 0111 0101 |

## Bytestream
0x49 0x10 0x0e 0x71

### Bitstream
<div style="width: 1em; height: 1em; background-color: #8e44ad; display:inline-block;"></div> <span style="color: #8e44ad">Node path</span>  
<br/>
<div style="width: 1em; height: 1em; background-color: #27ae60; display:inline-block;"></div> <span style="color: #27ae60">Element</span>  
<br>
<br>

| <span style="color:#27ae60">01001001</span> |  <span style="color:#8e44ad">0</span><span style="color:#27ae60">0010000</span> | <span style="color: #27ae60">0</span><span style="color:#8e44ad">00</span><span style="color:#27ae60">01110</span> | <span style="color:#27ae60">011</span><span style="color:#8e44ad">100</span><span style="color:#27ae60">01</span> |
| ------------ | ------------ | ------------ | ------------ |
| <span style="color:#27ae60">**I**</span>            |  0(<span style="color:#27ae60"> </span>) | 000(<span style="color:#27ae60">**s**</span>) | 011 100(<span style="color:#27ae60">**e**</span>)
