#if !defined(MAIN_H)
#define MAIN_H

void fade_from_black(const u8* palette, u8 delay);
void fade_to_black(const u8* palette, u8 delay);

extern u8 NumPlayers;
extern u8 Score;
extern u8 NumTerms;

#define MAYOR 0
#define DEPUTY 1
extern u8 Player1;
extern u8 Player2;

void meta_spr(u8 x, u8 y, u8 pal, const u8* data);

extern unsigned char* const metasprite_list[];

#endif // MAIN_H