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

### Part 1
0x49 0x10 0x0E 0x71 0x94 0x3c

### Part 2
0x63 0xAC 0x30 0xA3

<style>
div.box{
    display:inline-block;
    width: 1em;
    height: 1em;
}

div.box.node{
    background-color: #8e44ad;
}

div.box.element{
    background-color: #27ae60;
}

.node{
    color: #8e44ad;
}

.element{
    color: #27ae60;
}
</style>

### Bitstream
<div class="box node"></div> <span class="node">Node path</span>  
<br/>
<div class="box element"></div> <span class="element">Element</span>  
<br>
<br>

| <span class="element">0100 1001</span> | <span class="node">0</span><span class="element">001 0000</span> | <span class="element">0</span><span class="node">00</span><span class="element">0 1110</span> | <span class="element">011</span><span class="node">1 00</span><span class="element">01</span> | <span class="element">1001 01</span><span class="node">00</span> | <span class="node">0</span><span class="element">011 1100</span> |
| ------------ | ------------ | ------------ | ------------ | ------------ | ------------ |
| <span class="element">**I**</span>            |  0(<span class="element"> </span>) | 00(<span class="element">**s**</span>) | 011 100(<span class="element">**e**</span>) | 00 | 0(<span class="element">**x**</span>)

| <span class="element">0</span><span class="node">110 0</span><span class="element">011</span> | <span class="element">1010 1</span><span class="node">100</span> | <span class="node">0</span><span class="element">011 0000</span> | <span class="element">1</span> .... | | |
| ------------ | ------------ | ------------ | ------------ | ------------ | ------------ |
| 0 1100(<span class="element">**u**</span>) | 1000(a) |  | | | |
