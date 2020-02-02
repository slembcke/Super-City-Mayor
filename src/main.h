#if !defined(MAIN_H)
#define MAIN_H

void fade_from_black(const u8* palette, u8 delay);
void fade_to_black(const u8* palette, u8 delay);

extern u8 NumPlayers;
extern u8 Score;

unsigned char* const metasprite_list[];

#endif // MAIN_H