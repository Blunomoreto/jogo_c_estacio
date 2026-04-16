#ifndef PERSISTENCE_H
#define PERSISTENCE_H

typedef struct ScoreEntry {
	char name[24];
	int score;
	int wave;
} ScoreEntry;

void persistence_load_stats(int* highScore, int* maxWave);
void persistence_save_stats(int highScore, int maxWave);
void persistence_append_score(const char* name, int score, int wave);
int persistence_load_top_scores(ScoreEntry* outEntries, int maxEntries);
int persistence_load_scores(ScoreEntry* outEntries, int maxEntries);
void persistence_clear_scores(void);
void persistence_load_settings(int* audioEnabled, int* difficulty);
void persistence_save_settings(int audioEnabled, int difficulty);

#endif
