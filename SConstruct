def PhonyTargets(
    target,
    action,
    depends,
    env=None,
):
    # Creates a Phony target
    if not env:
        env = DefaultEnvironment()
    t = env.Alias(target, depends, action)
    env.AlwaysBuild(t)


PROJECT = "tiny-ladybug"
ELF = "{}.elf".format(PROJECT)
HEX = "{}.hex".format(PROJECT)

MCU = "attiny13"
FREQUENCY = str(1200000)

env_options = {
    "CC": "avr-gcc",
    "CPPDEFINES": ["F_CPU={}".format(FREQUENCY)],
    "CFLAGS": ["-std=c99", "-Wall", "-g", "-Os", "-mmcu={}".format(MCU)],
    "LINKFLAGS": ["-mmcu={}".format(MCU)]
}

env = DefaultEnvironment(**env_options)

sources = Glob("main/*.c")
sources += Glob("main/peripherals/*.c")

binary = env.Program(ELF, sources)
hexfile = env.Command(
    HEX, ELF, "avr-objcopy -j .text -j .data -O ihex {} {} ".format(ELF, HEX))

PhonyTargets(
    "size",  "avr-size --mcu={} --format=avr {}".format(MCU, ELF), binary, env)
PhonyTargets(
    "flash", "avrdude -p {} -c usbasp -B10 -U flash:w:{}:i -F -P usb".format(MCU, HEX), hexfile, env)
