#ifndef CHARACTER_ICON_CACHE_H
#define CHARACTER_ICON_CACHE_H

enum
{
    CHARACTER_ICON_CACHE_SIZE = 8,
};

struct GameTracker;
struct Icon;

void CharacterIconCache_Init(
    struct GameTracker *gGT);

void CharacterIconCache_LoadRosterPage(
    struct GameTracker *gGT,
    int page);

void CharacterIconCache_LoadRaceCharacters(
    struct GameTracker *gGT);

struct Icon *CharacterIconCache_Get(
    int characterID);

struct HighScoreEntry;
void CharacterIconCache_LoadHighScores(struct GameTracker *gGT,
    const struct HighScoreEntry *first, const struct HighScoreEntry *second);
struct Icon *CharacterIconCache_GetHighScore(int characterID);
int CharacterIconCache_GetNameColor(int characterID);

#endif
