INSTALLATION INSTRUCTIONS
------------ ------------

How to build banti from scratch.

1. Install `git`, `g++`, `png`, `jpeg`, `tiff`, `zlib` etc.
	```sh
	sudo apt-get install git g++ libpng-dev libjpeg-dev libtiff-dev libz-dev
	```

1. (Optional) Add [SSH keys](https://help.github.com/articles/generating-ssh-keys) if you want to develop and contribute.

1. Clone this project
	```sh
	git clone git@github.com:rakeshvar/banti.git
	```

1. Get latest version of Eclipse C++. Specific instructions for Ubuntu:
	1. Copy extracted directory to `/opt`
	1. Creat shortcut: `sudo ln -s -T /opt/eclipse/eclipse /usr/bin/eclipse`
	1. Add application by creating `/usr/share/applications/eclipse.desktop`
	1. If titles are not showing specify in the above file: `Exec=env UBUNTU_MENUPROXY= eclipse`

1. Run eclipse

1. Set workspace to the directory you cloned to.

1. `File -> Import -> General -> Existing Projects into Workspace`

	Specify the directory you cloned to 
	
	Check the option `Search for Nested Projects`

1. Install Freetype
	1. Download latest [source](http://sourceforge.net/projects/freetype/files/freetype2/)
	1. Unzip 
	1. Run the usual
	```sh
	./configure
	make -j4 
	sudo make install
	```

1. Build all the three `leptonica`, `gfft`, `banti`. Go to the `Project` menu and click `Build All` (`Ctrl B`).

	(ONLY) If you are getting Freetype errors, go to the properties of leptonica project.
	1. Right click on the `leptonica` project and select properties (`Alt Enter`) 
	2. Go to `C/C++ Build ->  Settings   -> Tool Settings -> GCC Compiler -> Includes` and add `/usr/local/include` and `/usr/local/include/freetype2`

1. Run `<path_to_git_directory>/banti/Debug/banti` to see full options. 

	Increase stack size if you are getting a Seg Fault. 
	```sh
	ulimit -s 1000000
	```

1. If you want to run classifier you will need to have the data (`charcodes.txt`, `cp.bin`, `sm.bin`, `wr.bin`) in the data directory (symbolically) located in the same folder as the executable. 
	```sh
	<path_to_git_directory>//banti/Debug$ ln -s -T ../../data/ data
	```
