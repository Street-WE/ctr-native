#ifndef HIGH_SCORE_REGISTRY_H
#define HIGH_SCORE_REGISTRY_H

struct LevelDef;
struct HighScoreTrack;
struct HighScoreTrack *HighScoreRegistry_Get(int levelID, const struct LevelDef *level);
struct HighScoreTrack *HighScoreRegistry_GetActive(void);
void HighScoreRegistry_SaveActive(void);

#endif
