# CHIP-8 Emulator

Firstly, a breif overview on what this exactly is. The given code is my attempt at CPU emulation using
C++. The CHIP8 has 16 8 bit general purpose registers (represented in my code by V[15]). I is the Index register, PC or Program Counter stores the instructions to be executed next by the CPU. Stack and Stack pointer are used for returning to the main program after executing a specific sub routine. Sound timer beeps when it reaches 0 and delayTimer has other use cases. I have not incorporated them in our code and most opcodes ive implemented do not require them. Display is used for simulating a simple display. When drawflag is set to 1 it means the display needs to be updated

# Basic flow  of Execution 

Initially, we reset everything to its default state. The function ``loadProgram`` is used to take the hex opcodes from the main function and perform required operations depending on the case it falls other. All the opcodes and their functions have been defined under the function ``executeCycle``. I have focussed on implemented 7 opcodes. They are as follows:

- 00E0	Clear the screen (CLS)
- 00EE	Return from subroutine (RET)
- 1NNN	Jump to address NNN (JP addr)
- 2NNN	Call subroutine at NNN (CALL addr)
- 3XNN	Skip next instruction if Vx == NN (SE Vx, NN)
- 6XNN	Load NN into Vx (LD Vx, NN)
- 7XNN	Add NN to Vx (ADD Vx, NN)*/

The ``run()`` function keeps the``executeCycle`` function running in the main function. ``drawScreen()`` function is used to replicate a simple 2D display with just 2 states, ie ON/OFF. Have not tested this part of the code yet and will work on it by incorportating a proper GUI. 

The functions ``saveState()`` and ``loadState()`` are used to save and load state respectively. It writes the contents of the registers to a binary file. The ``loadState()`` is used to retrive the contents of the file and resume from where the user left off.
# Issues for Now

Because of MinGW, this_thread does not work on Windows because it not enabled my default. Or could be a compiler thing i am not sure. Works on Ubuntu 22.04.

# Things that can be Improved

Can make a GUI for displaying content. Might work on this sometime later




