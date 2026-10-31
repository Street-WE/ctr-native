#include <common.h>
#include <LevelRegistry.h>
#include <HighScoreRegistry.h>
#include <platform/native_memcard.h>

// Keep the retail memory-card layout intact. Additional tracks use stable manifest IDs.
struct AdditionalHighScores
{
    char id[LEVEL_REGISTRY_ID_LENGTH];
    struct HighScoreTrack track;
};

static struct AdditionalHighScores sAdditionalHighScores[32];
static char sAdditionalHighScoreFiles[32][32];

struct HighScoreTrack *HighScoreRegistry_Get(int levelID, const struct LevelDef *level)
{
    if (level == NULL || level->replaceLevelID >= 0)
        return &sdata->gameProgress.highScoreTracks[levelID];

    for (unsigned int i = 0; i < len(sAdditionalHighScores); i++)
        if (strcmp(sAdditionalHighScores[i].id, level->id) == 0)
            return &sAdditionalHighScores[i].track;

    for (unsigned int i = 0; i < len(sAdditionalHighScores); i++)
    {
        struct AdditionalHighScores *scores = &sAdditionalHighScores[i];
        if (scores->id[0] != 0) continue;
        // A filename-safe, order-independent key; retain the full ID in the file as well.
        unsigned long long hash = 14695981039346656037ull;
        for (const unsigned char *ch = (const unsigned char *)level->id; *ch; ch++)
            hash = (hash ^ *ch) * 1099511628211ull;
        snprintf(sAdditionalHighScoreFiles[i], sizeof(sAdditionalHighScoreFiles[i]),
            "CTR-HS-%016llx", hash);
        struct AdditionalHighScores saved;
        if (NativeMemcard_ReadSaveData(sAdditionalHighScoreFiles[i], (unsigned char *)&saved,
                sizeof(saved), 0) == NATIVE_MEMCARD_OK &&
            memcmp(saved.id, level->id, sizeof(saved.id)) == 0)
        {
            *scores = saved;
            for (int j = 0; j < MEMCARD_HIGH_SCORE_ENTRIES_PER_TRACK; j++)
                scores->track.scoreEntry[j].name[MEMCARD_HIGH_SCORE_NAME_LENGTH - 1] = 0;
            return &scores->track;
        }
        strcpy(scores->id, level->id);
        for (int j = 0; j < MEMCARD_HIGH_SCORE_ENTRIES_PER_TRACK; j++)
        {
            scores->track.scoreEntry[j].time = MEMCARD_HIGH_SCORE_DEFAULT_TIME;
            scores->track.scoreEntry[j].characterID = CRASH_BANDICOOT;
            strcpy(scores->track.scoreEntry[j].name, "---");
        }
        return &scores->track;
    }
    // The level registry also caps loaded manifests at 32.
    static struct HighScoreTrack overflow;
    return &overflow;
}

struct HighScoreTrack *HighScoreRegistry_GetActive(void)
{
    int levelID = sdata->gGT->levelID;
    const struct LevelDef *level = LevelRegistry_GetActive();
    if (level != NULL && level->baseLevelID != levelID) level = NULL;
    return HighScoreRegistry_Get(levelID, level);
}

void HighScoreRegistry_SaveActive(void)
{
    const struct LevelDef *level = LevelRegistry_GetActive();
    if (level == NULL || level->replaceLevelID >= 0 ||
        level->baseLevelID != sdata->gGT->levelID) return;
    for (unsigned int i = 0; i < len(sAdditionalHighScores); i++)
    {
        if (strcmp(sAdditionalHighScores[i].id, level->id) != 0) continue;
        if (NativeMemcard_WriteSaveData(sAdditionalHighScoreFiles[i], "", 0,
                (const unsigned char *)&sAdditionalHighScores[i], sizeof(sAdditionalHighScores[i])) != NATIVE_MEMCARD_OK)
            fprintf(stderr, "Could not save additional track highscores for %s\n", level->id);
        return;
    }
}
