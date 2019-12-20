
## Debugging the application
Compile the app only with the -verbose option to see where the executeable is being created.
```
petalinux-build -c xroe-app -v -x compile
```

Now you can scp it to the board once compiled. This results in the fastest development spin cycle.
```
scp /tmp/om0_pl00-2019.09.16-13.46.47-qPL/work/aarch64-xilinx-linux/xroe-app/1.0-r0/xroe-app root@xxx.xxx.xxx.xxx/.
```
