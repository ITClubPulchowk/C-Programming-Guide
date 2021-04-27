# Calculator
This isn't even a full calculator. It evaluates simple expression and supports following operator with given precedence : <br><br>
<pre>
parenthesis     -> ( ) <br>
unaray operator -> + - <br>
exponentiation  -> ^   <br>
mul / div       -> / * <br>
binary add/sub  -> + - <br>
assignment      -> =   <br>
</pre>
All operators are left assosciative except exponentiation ('^') and assignment ('=') operator. <br>
There's no support for comparison or any bitwise operator.
<img src="./includes/calc1.png"> <br>
Simple error messages are displayed. So, don't make hard errors. <br>
<img src="./includes/calc2.png">
<br> Note : No color support for windows (>.<) <br>
# Build 
### For windows : <br>
`build.bat`
<br>
It will generate bin/ folder if it doesn't exist and put executable inside bin.
<br>
Run<br> 
`"bin\calculator.exe"`  or
<br>
`cd bin` <br>
`calculator.exe`<br>
<br>
<br>
### For linux : 
<br> Execute <br> `chmod u+x build.sh`<br> to make it executable <br>
Build using `./build.sh`
<br> Run <br> `./bin/calculator`
<br>
