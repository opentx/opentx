CooCox CoOS's source code package includes the following files:

kernel: CooCox CoOS's source code, you can use it without any modification.

portable: CooCox CoOS's port for Cortex M3. It includes the source code for Cortex M3 architecture with three edition : MDK, IAR, GCC. You can choose a suitable one for your application.

config: Configuration files for CooCox CoOS. You can tailor CooCox CoOS for your application with it.

CooCox CoOS's TERMS AND CONDITIONS.pdf: The terms you should obey when using CooCox CoOS's source code.

To develop application based on CooCox CoOS, you should include all the source code files in the kernel folder, and choose an edition in portable folder for the compiler that you are using. You also should amend config.h for tailoring CooCox CoOS, otherwise CooCox CoOS may be too big for your needs.
