/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#ifndef BOARD_H
#define BOARD_H

#include <lxrad.h>
#include <picsim/picsim.h>
#include <stdint.h>

#define INCOMPLETE                                                      \
    printf("Incomplete: %s -> %s :%i\n", __func__, __FILE__, __LINE__); \
    exit(-1);

enum { ARCH_P16, ARCH_P16E, ARCH_P18, ARCH_AVR8, ARCH_STM32, ARCH_STM8, ARCH_C51, ARCH_Z80, ARCH_UNKNOWN };

/**
 * @brief input map struct
 *
 */
typedef struct {
    unsigned int x1;        ///< x1 position
    unsigned int x2;        ///< x2 position
    unsigned int y1;        ///< y1 position
    unsigned int y2;        ///< y2 position
    unsigned int cx;        ///< center x position
    unsigned int cy;        ///< center y position
    char name[10];          ///< region name
    unsigned short id;      ///< region ID
    void* status;           ///< rcontrol status
    unsigned char* update;  ///< output need draw update
    union {
        unsigned char value;  ///< updated value
        short value_s;        ///< updated value short
        float value_f;        ///< updated value float
    };
} input_t;

/**
 * @brief output map struct
 *
 */
typedef struct {
    unsigned int x1;  ///< x1 position
    unsigned int x2;  ///< x2 position
    unsigned int y1;  ///< y1 position
    unsigned int y2;  ///< y2 position
    unsigned int cx;  ///< center x position
    unsigned int cy;  ///< center y position
    unsigned int r;
    char name[10];         ///< region name
    unsigned short id;     ///<  region ID
    void* status;          ///< rcontrol status
    unsigned char update;  ///< need draw update
    union {
        unsigned char value;  ///< updated value
        short value_s;        ///< updated value short
        float value_f;        ///< updated value float
    };
} output_t;

#define MAX_TIMERS 256

/**
 * @brief internal timer struct
 *
 */
typedef struct {
    uint32_t Timer;
    uint32_t Reload;
    void* Arg;
    void (*Callback)(void* arg);
    int Enabled;
} Timers_t;

/**
 * @brief Board class
 *
 * class definition of boards used in picsimlab.
 */
class board {
public:
    /**
     * @brief Called ever 100ms to draw board
     */
    virtual void Draw(CDraw* draw) = 0;

    /**
     * @brief Paralle thread called ever 100ms to run cpu code
     */
    virtual void Run_CPU(void) = 0;

    /**
     * @brief  Return a list of supported microcontrollers
     */
    virtual lxString GetSupportedDevices(void) = 0;

    /**
     * @brief Return the filename of board picture
     */
    virtual lxString GetPictureFileName(void);

    /**
     * @brief  Return the filename of board picture map
     */
    virtual lxString GetMapFile(void);

    /**
     * @brief  Reset board status
     */
    virtual void Reset(void) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvMouseButtonPress(uint button, uint x, uint y, uint state) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvMouseButtonRelease(uint button, uint x, uint y, uint state) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvMouseMove(uint button, uint x, uint y, uint state){};

    /**
     * @brief  Event on the board
     */
    virtual void EvKeyPress(uint key, uint mask) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvKeyRelease(uint key, uint mask) = 0;

    /**
     * @brief  Event on the board
     */
    virtual void EvOnShow(void);

    /**
     * @brief  Event on the board
     */
    virtual void EvThreadRun(CThread& thread){};

    /**
     * @brief  Called ever 1s to refresh status
     */
    virtual void RefreshStatus(void);

    /**
     * @brief  Called to save board preferences in configuration file
     */
    virtual void WritePreferences(void){};

    /**
     * @brief  Called whe configuration file load  preferences
     */
    virtual void ReadPreferences(char* name, char* value){};

    /**
     * @brief  return the input ids numbers of names used in input map
     */
    virtual unsigned short GetInputId(char* name) = 0;

    /**
     * @brief  return the output ids numbers of names used in output map
     */
    virtual unsigned short GetOutputId(char* name) = 0;

    /**
     * @brief  Called when window side controls are activated
     */
    virtual void board_Event(CControl* control){};

    /**
     * @brief  Called when window side controls are activated
     */
    virtual void board_ButtonEvent(CControl* control, uint button, uint x, uint y, uint state){};

    /**
     * @brief  Called once on board creation
     */
    board(void);

    /**
     * @brief  Called once on board destruction
     */
    virtual ~board(void);

    /**
     * @brief  Enable/disable oscilloscope measurement
     */
    void SetUseOscilloscope(int uo);

    /**
     * @brief  Enable/disable spare parts
     */
    void SetUseSpareParts(int sp);

    /**
     * @brief Get if oscilloscope is in use
     */
    int GetUseOscilloscope(void);

    /**
     * @brief Get if spare parts is in use
     */
    int GetUseSpareParts(void);

    /**
     * @brief Get board name registered in PICSimLab
     */
    virtual lxString GetName(void) = 0;

    /**
     * @brief Set board processor
     */
    void SetProcessorName(lxString proc);

    /**
     * @brief Get board processor in use
     */
    lxString GetProcessorName(void);

    /**
     * @brief Get board input count
     */
    int GetInputCount(void);

    /**
     * @brief Get board input
     */
    input_t* GetInput(int n);

    /**
     * @brief Get board output count
     */
    int GetOutputCount(void);

    /**
     * @brief Get board input
     */
    output_t* GetOutput(int n);

    /**
     * @brief Start debug support
     */
    virtual int DebugInit(int dtyppe) = 0;

    /**
     * @brief Get debug interface name
     */
    virtual lxString GetDebugName(void) = 0;

    /**
     * @brief  Return the about information of part
     */
    virtual lxString GetAboutInfo(void) = 0;

    /**
     * @brief debug step (pooling)
     */
    virtual void DebugLoop(void) = 0;

    /**
     * @brief return true if microcontroller is initialized
     */
    virtual int CpuInitialized(void) = 0;

    /**
     * @brief Set serial port name to use
     */
    virtual void MSetSerial(const char* port) = 0;

    /**
     * @brief board microcontroller init
     */
    virtual int MInit(const char* processor, const char* fname, float freq) = 0;

    /**
     * @brief board microcontroller end
     */
    virtual void MEnd(void) = 0;

    /**
     * @brief Return board microcontroller architecture
     */
    virtual int MGetArchitecture(void) = 0;

    /**
     * @brief board servers shutdown
     */
    virtual void EndServers(void){};

    /**
     * @brief board microcontroller save non volatile memory to hex file
     */
    virtual void MDumpMemory(const char* fname) = 0;

    /**
     * @brief board microcontroller erase flash memory (program)
     */
    virtual void MEraseFlash(void) = 0;

    /**
     * @brief board microcontroller set frequency
     */
    virtual void MSetFreq(float freq) = 0;

    /**
     * @brief board microcontroller get frequency
     */
    virtual float MGetFreq(void) = 0;

    /**
     * @brief board microcontroller set vcc
     */
    virtual void MSetVCC(float vcc) = 0;

    /**
     * @brief board microcontroller get vcc
     */
    virtual float MGetVCC(void) = 0;

    /**
     * @brief board microcontroller get cpu internal clock (in PIC frequency/4)
     */
    virtual float MGetInstClockFreq(void) = 0;

    /**
     * @brief board microcontroller pin count
     */
    virtual int MGetPinCount(void) = 0;

    /**
     * @brief board microcontroller pin name
     */
    virtual lxString MGetPinName(int pin) = 0;

    /**
     * @brief board microcontroller set digital pin
     */
    virtual void MSetPin(int pin, unsigned char value) = 0;

    /**
     * @brief board microcontroller set Default Open Value (external pull)
     */
    virtual void MSetPinDOV(int pin, unsigned char ovalue) = 0;

    /**
     * @brief board microcontroller set analog pin
     */
    virtual void MSetAPin(int pin, float value) = 0;

    /**
     * @brief board microcontroller get digital pin value
     */
    virtual unsigned char MGetPin(int pin) = 0;

    /**
     * @brief board microcontroller get all pins list struct
     */
    virtual const picpin* MGetPinsValues(void) = 0;

    /**
     * @brief board microcontroller run one step
     */
    virtual void MStep(void) = 0;

    /**
     * @brief board microcontroller run one or two steps to resume instruction
     */
    virtual void MStepResume(void) = 0;

    /**
     * @brief board microcontroller reset
     */
    virtual void MReset(int flags) = 0;

    /**
     * @brief board microcontroller get pointer to processor ID
     */
    virtual unsigned short* DBGGetProcID_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get PC
     */
    virtual unsigned int DBGGetPC(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller set PC
     */
    virtual void DBGSetPC(unsigned int pc) { INCOMPLETE; };

    /**
     * @brief  board microcontroller get RAM memory pointer
     */
    virtual unsigned char* DBGGetRAM_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get ROM (FLASH) memory pointer
     */
    virtual unsigned char* DBGGetROM_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller  get CONFIG memory pointer
     */
    virtual unsigned char* DBGGetCONFIG_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller  get internal IDS memory pointer
     */
    virtual unsigned char* DBGGetID_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get data EEPROM memory pointer
     */
    virtual unsigned char* DBGGetEEPROM_p(void) {
        INCOMPLETE;
        return NULL;
    };

    /**
     * @brief  board microcontroller get RAM memory size
     */
    virtual unsigned int DBGGetRAMSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get ROM  (FLASH) memory size
     */
    virtual unsigned int DBGGetROMSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief board microcontroller get CONFIG memory size
     */
    virtual unsigned int DBGGetCONFIGSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief board microcontroller get internal IDS memory size
     */
    virtual unsigned int DBGGetIDSize(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get data EEPROM memory size
     */
    virtual unsigned int DBGGetEEPROM_Size(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get last ram write address
     */
    virtual unsigned int DBGGetRAMLAWR(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  board microcontroller get last ram read address
     */
    virtual unsigned int DBGGetRAMLARD(void) {
        INCOMPLETE;
        return 0;
    };

    /**
     * @brief  Calc rotary potentiometer angle
     */
    unsigned char CalcAngle(int i, int x, int y);

    /**
     * @brief  Set board draw scale
     */
    virtual void SetScale(double scale);

    /**
     * @brief  Get board draw scale
     */
    double GetScale(void);

    /**
     * @brief  Get board default clock in MHz
     */

    virtual int GetDefaultClock(void) { return 2; };

    /**
     * @brief  Get board default processor
     */

    lxString GetDefaultProcessor(void) { return DProc; };

    /**
     * @brief  Set board default processor
     */

    void SetDefaultProcessor(lxString dproc) { DProc = dproc; };

    /**
     * @brief Get instruction counter
     */
    uint32_t InstCounterGet(void) { return InstCounter; };

    /**
     * @brief Get elapsed time from instruction counter in us
     */
    uint32_t InstCounterGet_us(const uint32_t start);

    /**
     * @brief Get elapsed time from instruction counter in us
     */
    uint32_t InstCounterGet_ms(const uint32_t start);

    /**
     * @brief Register a new timer with time in us (default enabled)
     */
    int TimerRegister_us(const double micros, void (*Callback)(void* arg), void* arg);

    /**
     * @brief Register a new timer with time in ms (default enabled)
     */
    int TimerRegister_ms(const double miles, void (*Callback)(void* arg), void* arg);

    /**
     * @brief Unregister timer
     */
    int TimerUnregister(const int timer);

    /**
     * @brief Modify timer value with us
     */
    int TimerChange_us(const int timer, const double micros);

    /**
     * @brief Modify timer value with us
     */
    int TimerChange_ms(const int timer, const double miles);

    /**
     * @brief Enable or disable timer
     */
    int TimerSetState(const int timer, const int enabled);

    /**
     * @brief Get elapsed time from instruction counter in ns
     */
    uint64_t TimerGet_ns(const int timer);

protected:
    /**
     * @brief Register remote control variables
     */
    virtual void RegisterRemoteControl(void){};

    /**
     * @brief Increment the Intructions Counter
     */
    void InstCounterInc(void);

    lxString Proc;              ///< Name of processor in use
    lxString DProc;             ///< Name of default board processor
    input_t input[120];         ///< input map elements
    input_t* input_ids[120];    ///< input map elements by id order
    output_t output[120];       ///< output map elements
    output_t* output_ids[120];  ///< output map elements by id order
    int inputc;                 ///< input map elements counter
    int outputc;                ///< output map elements counter
    int use_oscope;             ///< use oscilloscope window
    int use_spare;              ///< use spare parts window
    unsigned char p_RST;        ///< board /RESET pin state
    double Scale;

    /**
     * @brief  Read maps
     */
    void ReadMaps(void);

    /**
     * @brief  Start parallel thread
     */
    void StartThread(void);

private:
    uint32_t InstCounter;
    int TimersCount;
    Timers_t Timers[MAX_TIMERS];
    Timers_t* TimersList[MAX_TIMERS];

    /**
     * @brief Read the Input Map
     */
    void ReadInputMap(lxString fname);

    /**
     * @brief Read the Output Map
     */
    void ReadOutputMap(lxString fname);
};

extern int ioupdated;

#endif /* BOARD_H */

#ifndef BOARDS_DEFS_H
#define BOARDS_DEFS_H

#define board_init(name, function)                                  \
    static board* function##_create(void) {                         \
        board* b = new function();                                  \
        b->SetDefaultProcessor(b->GetProcessorName());              \
        return b;                                                   \
    };                                                              \
    static void __attribute__((constructor)) function##_init(void); \
    static void function##_init(void) { board_register(name, function##_create); }

typedef board* (*board_create_func)(void);

void board_register(const char* name, board_create_func bcreate);

// boards object creation
board* create_board(int* lab, int* lab_);

#define BOARDS_MAX 20

extern int BOARDS_LAST;

typedef struct {
    char name[30];   // name
    char name_[30];  // name without spaces
    board_create_func bcreate;
} board_desc;

extern board_desc boards_list[BOARDS_MAX];

// Arduino Uno is the dafault board
#define DEFAULT_BOARD 2

#endif /* BOARDS_DEFS_H */
