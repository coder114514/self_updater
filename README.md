# Self Updater

A windows counter program that updates itself everytime it's runned. The counter is stored at the end of the exe.

For example, if the exe is called `main.exe`, every time it is runned, it will create a message box that displays the counter, and the counter will increment every time it is runned.

`/R` or `/r` switches can reset the counter to 0. (`main.exe /R` or `main.exe /r`).

Do not do it too fast, or it will break itself.

# How does it work

It creates a copy of itself and then runs the copy to update the original exe.

# How to compile

`cl.exe main.cpp`
