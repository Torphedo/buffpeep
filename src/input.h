#include <stdbool.h>

#include <GLFW/glfw3.h>

// TLDR: We have to keep state ourselves because relying on the callback makes
// it feel awful.

// This exists because the callback only triggers when the key state changes.
// So if we completely depend on it for input, we get a ~0.5 second gap between
// when it's considered "pressed" and "held". The end behaviour is that holding
// a key will trigger an action once, then do nothing for a half second until
// it's considered held. To avoid this we keep track of key state ourselves, so
// that the key appears held the entire time.

// Using bitfield saves some space.
typedef struct {
    bool space: 1;
    bool apostrophe: 1;
    bool comma: 1;
    bool minus: 1;
    bool period: 1;
    bool slash: 1;
    bool num_0: 1;
    bool num_1: 1;
    bool num_2: 1;
    bool num_3: 1;
    bool num_4: 1;
    bool num_5: 1;
    bool num_6: 1;
    bool num_7: 1;
    bool num_8: 1;
    bool num_9: 1;
    bool semicolon: 1;
    bool equal: 1;
    bool a: 1;
    bool b: 1;            
    bool c: 1;            
    bool d: 1;            
    bool e: 1;            
    bool f: 1;            
    bool g: 1;            
    bool h: 1;            
    bool i: 1;            
    bool j: 1;            
    bool k: 1;            
    bool l: 1;            
    bool m: 1;            
    bool n: 1;            
    bool o: 1;            
    bool p: 1;            
    bool q: 1;            
    bool r: 1;            
    bool s: 1;            
    bool t: 1;            
    bool u: 1;            
    bool v: 1;            
    bool w: 1;            
    bool x: 1;            
    bool y: 1;            
    bool z: 1;            
    bool left_bracket: 1;
    bool backslash: 1;
    bool right_bracket: 1;
    bool grave_accent: 1;
    bool world_1: 1;      
    bool world_2: 1;      

    bool escape: 1;
    bool enter: 1;
    bool tab: 1;
    bool backspace: 1;
    bool insert: 1;
    bool delete: 1;
    bool right: 1;
    bool left: 1;
    bool down: 1;
    bool up: 1;
    bool page_up: 1;
    bool page_down: 1;
    bool home: 1;
    bool end: 1;
    bool caps_lock: 1;
    bool scroll_lock: 1;
    bool num_lock: 1;
    bool print_screen: 1;
    bool pause: 1;
    bool f1: 1;           
    bool f2: 1;           
    bool f3: 1;           
    bool f4: 1;           
    bool f5: 1;           
    bool f6: 1;           
    bool f7: 1;           
    bool f8: 1;           
    bool f9: 1;           
    bool f10: 1;          
    bool f11: 1;          
    bool f12: 1;          
    bool f13: 1;          
    bool f14: 1;          
    bool f15: 1;          
    bool f16: 1;          
    bool f17: 1;          
    bool f18: 1;          
    bool f19: 1;          
    bool f20: 1;          
    bool f21: 1;          
    bool f22: 1;          
    bool f23: 1;          
    bool f24: 1;          
    bool f25: 1;          
    bool numpad_0: 1;         
    bool numpad_1: 1;         
    bool numpad_2: 1;         
    bool numpad_3: 1;         
    bool numpad_4: 1;         
    bool numpad_5: 1;         
    bool numpad_6: 1;         
    bool numpad_7: 1;         
    bool numpad_8: 1;         
    bool numpad_9: 1;         
    bool numpad_decimal: 1;
    bool numpad_divide: 1;
    bool numpad_multiply: 1;
    bool numpad_subtract: 1;
    bool numpad_add: 1;
    bool numpad_enter: 1;
    bool numpad_equal: 1;
    bool left_shift: 1;
    bool left_control: 1;
    bool left_alt: 1;
    bool left_super: 1;
    bool right_shift: 1;
    bool right_control: 1;
    bool right_alt: 1;
    bool right_super: 1;
    bool menu: 1;

    bool shift: 1;
    bool control: 1;
    bool alt: 1;
    bool super: 1;
}input_internal;

// The global input struct our callback will update
extern input_internal input;

void update_input_callback(GLFWwindow* window, int key, int scancode, int actions, int mods);

