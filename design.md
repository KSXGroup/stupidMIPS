# Ideas of MIPS Simulator#

## 1.Memory Layout

​	 http://www.cs.uwm.edu/classes/cs315/Bacon/Lecture/HTML/ch10s04.html

   >10.4. Memory Segments
   >
   >The Von Neumann architecture is defined by the fact that both the machine instructions and data are stored in the same memory space. This fact necessitates dividing memory into text and data segments.
   >
   >In modern operating systems, there are more than just these two segments. Most systems use a single text segment, but actually use up to three data segments, depending on the storage class of the data being stored there. The four segments can be described as follows:
   >
   >    The code segment, also known as text segment contains the machine instructions of the program. The code can be thought of like the text of a novel: It tells the story of what the program does.
   >    The data segment contains the static data of the program, i.e. the variables that exist throughout program execution. Global variables in a C or C++ program are static, as are variables declared as static in C, C++, or Java.
   >    The stack segment contains the system stack, which is used as temporary storage. The stack is a simple data structure with a LIFO (last-in first-out) access policy. Items are only added to or removed from the "top" of the stack. Implementing a stack requires only a block of memory (e.g. an array in a HLL) and a stack pointer which tells us where the top of the stack is. In the MIPS architecture, the $sp register is designated as the stack pointer. Adding an element to the top of the stack is known as a push, and retrieving an item from the top is known as a pop. If we were to push the values 1, 2, 3, 4, and 5, and then do a single pop, the stack would appear as follows:
   >    
   >    	    +---+
   >    	    | 5 |
   >    	    | 4 | <- sp
   >    	    | 3 |
   >    	    | 2 |
   >    	    | 1 |
   >    	    +---+
   >
   >
   >    The heap segment is a pool of memory used for dynamically allocated memory, such as with malloc() in C or new in C++ and Java. 
   >
   >The size of the text and data segments are known as soon as compilation or assembly is completed. The stack and heap segments, on the other hand, grow and shrink during program execution. For this reason, they tend to be configured such that they grow toward each other. This way, they each have a fixed starting point (one adjacent to the text and data segments, and the other at one end of the entire memory space), and the boundary between them is flexible. Both can grow until all available memory is used.
   >
   >	+-----------+
   >	|           |
   >	|   text    | (fixed size)
   >	|           |
   >	+-----------+
   >	|           |
   >	|   data    | (fixed size)
   >	|           |
   >	+-----------+
   >	|   stack   | | growth
   >	+-----------+ V
   >	|           |
   >	|   free    |
   >	|           |
   >	+-----------+ ^
   >	|   heap    | | growth
   >	+-----------+
   >
   >
   >We can have as many .text and .data blocks in the source code as we want. The assembler will consolidate all the .text blocks into the text segment, and and all the .data blocks into the data segment.
   >
   >Each subprogram in a MAL program should have its own .text block and its own .data block. 

http://www.it.uu.se/education/course/homepage/os/vt18/module-0/mips-and-mars/mips-memory-layout/

>![img](/home/kstarxin/code/mips/design/memlayout.png)
>
>



## 2. Instruction Reference

https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_help.html