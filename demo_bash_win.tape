# demo_bash_clean.tape
Output demo.gif
Set Theme "OneHalfDark"
Set FontFamily "Cascadia Mono"
Set FontSize 20
Set Width 1280
Set Height 800
Set PlaybackSpeed 0.95
Set Shell "bash"

Type "export PS1='$ '"
Enter
Type "clear"
Enter
Sleep 200ms

Type "printf 'Hello World!' > sample.bin"
Enter
Sleep 250ms
Type "head -c 4096 /dev/urandom > big.bin"
Enter
Sleep 350ms

Type "./hexview.exe -v"
Enter
Sleep 700ms

Type "./hexview.exe sample.bin"
Enter
Sleep 900ms

Type "./hexview.exe sample.bin --compact --color=always"
Enter
Sleep 900ms

Type "./hexview.exe sample.bin -w 32 --no-ascii"
Enter
Sleep 900ms

Type "./hexview.exe big.bin -o 0x100 -l 64 -w 16 --compact --color=always"
Enter
Sleep 1100ms

Type "cat big.bin | ./hexview.exe - --compact"
Enter
Sleep 1100ms
