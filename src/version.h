
#define MAJOR APPVERSION_M
#define MINOR APPVERSION_N
#define PATCH APPVERSION_P

typedef struct version {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} version_t;

const version_t version = { MAJOR, MINOR, PATCH };
