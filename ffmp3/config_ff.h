#ifndef arm

#define ARCH_X86 1
#define CONFIG_MPEGAUDIO_HP 1
#define CONFIG_AUDIO_OSS 1
#define restrict __restrict__

#else //arm

#define CONFIG_DECODERS 1
#define CONFIG_AUDIO_OSS 1
#define CONFIG_NETWORK 1
#define HAVE_MALLOC_H 1
#define HAVE_MEMALIGN 1
#define SIMPLE_IDCT 1

#endif //arm
