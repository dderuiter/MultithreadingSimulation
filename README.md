# MultithreadingSimulation

OVERVIEW:
Simple demonstration of multithreading, how it can generate race conditions, and how to solve race conditions using mutexes.

DETAILS:
Author: Davis DeRuiter
Date: 11/24/2016
School: Liberty University
Course: CSIS 443-001
Assignment: Final Project

HOW TO RUN:

1) Open up the Command Prompt in the Windows OS.

2) Navigate to the unzipped directory "DavisDeRuiterCSIS443".

3) Two executables are provided:
	a) Run the file "MultiThreadingSimulation.exe" if Visual Studio is installed on your computer.
           This version does not include the statically linked dll. So if run on a computer without Visual Studio, an
           error will be shown stating that the computer is missing MSVCP140.dll.
	b) Run the file "MultiThreadingSimulation_Linked.exe" if Visual Studio is not installed on your computer.
           This version statically links the necessary dlls, so the executable is bigger in size but it allows for running
           on a computer without Visual Studio installed. This version performs better than the prior, so the number
           of iterations must be increased in order to generate race conditions.
   
4) When running either executable specify the desired options for the run.   
   i.e. "MultiThreadingSimulation.exe 98 1000000 1"
 (98 - thread count, 1000000 - iteration count, 1 - mutex enabled)
 
5) Wait for the program execution to end.
 
6) Execution ends when the user is prompted for a key
   i.e. "Enter any key to exit: "
