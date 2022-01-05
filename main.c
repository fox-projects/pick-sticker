#include "./third_party/raylib/include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include  <errno.h>

// lmfao
#define LOAD(_n, _file) const char *file##_n = _file; \
  Image image##_n = LoadImage(file##_n); \
  ImageResize(&image##_n, gridSize, gridSize); \
  Texture2D texture##_n = LoadTextureFromImage(image##_n); \
  shown_textures[2] = &texture##_n

/**
 * @brief Print error message and die
 */
_Noreturn static void die(char *msg) {
  fprintf(stderr, "%s", "Error: ");
  fprintf(stderr, "%s", msg);
  fprintf(stderr, "%s\n", ". Exiting");
  exit(1);
}

/**
 * @brief Determine if a string ends with a specific file extension
 */
static inline bool has_extension(const char *name, const char *ext)
{
  size_t nl = strlen(name);
  size_t el = strlen(ext);
  return nl >= el && !strcmp(name + nl - el, ext);
}

/**
 * @brief Create a index file
 */
static void create_index(char* index_file_name, char *search_dir) {
  FILE *index_file = fopen(index_file_name, "w+");
  if (!index_file) {
    die("Could not open index file");
  }

  // TODO
  #define _BSD_SOURCE
  DIR *d = opendir(search_dir);
  if (d) {
    struct dirent *dir;
    while((dir = readdir(d)) != NULL) {
      if (dir->d_type == DT_DIR) {
        if (strcmp(dir->d_name, ".") == 0) continue;
        if (strcmp(dir->d_name, "..") == 0) continue;

        char s[PATH_MAX];
        strcpy(s, search_dir);
        strncat(s, "/", 1);
        strcat(s, dir->d_name);

        DIR *subd = opendir(s);
        if (subd) {
          struct dirent *subdir;
          while((subdir = readdir(subd)) != NULL) {
            if (subdir->d_type == DT_REG) {

              char ss[PATH_MAX];
              strcpy(ss, s);
              strncat(ss, "/", 1);
              strcat(ss, subdir->d_name);

              if (has_extension(ss, ".png")) {
                fprintf(index_file, "%s:\n", ss);
              }
            }
          }
          closedir(subd);
        } else {
          fprintf(stderr, "%s\n", "Unexpected NULL pointer");
        }
      }
    }
    closedir(d);
  } else {
    fprintf(stderr, "%s\n", "Unexpected NULL pointer");
  }

  fclose(index_file);
}

static void search_images(char *root_dir, char *file, struct dirent *dir, struct dirent *subdir, int *i) {
}

static Texture2D get_texture(const char *file, const int imageSize) {
  Image image = LoadImage(file);
  ImageResize(&image, imageSize, imageSize);
  Texture2D texture = LoadTextureFromImage(image);
  return texture;
}

int main() {
  /* create index */
  create_index("./index_file", "./downloader/downloads");

  /* constants */
  const int imageSize = 100;
  const int imageMargin = 5;
  const int imageSizeTotal = imageSize + (imageMargin * 2);

  const int gridColumns = 9;
  const int gridRows = 7;

  const int windowBorder = 5;
  const int windowWidth = (gridColumns * (imageSizeTotal)) + (windowBorder * 2);
  const int windowHeight = 778 + windowBorder;

  const struct Vector2 gridStart = (Vector2){ windowBorder, 80 };

  const struct Color colorBlack = (Color){ 52, 58, 64, 255 };
  const struct Color colorLightGray = (Color){ 233, 236, 239, 255 };
  const struct Color colorDarkGray = (Color){ 173, 181, 189, 255 };
  const struct Color colorWhite = WHITE;

  /* start */
  InitWindow(windowWidth, windowHeight, "Pick Sticker");

  /* load images and their attributes into memory */
  #define SHOWN_LENGTH 100
  char* shown_files[SHOWN_LENGTH];
  Texture2D shown_textures[SHOWN_LENGTH];
  FILE *index_file = fopen("./index_file", "r");
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int i = 0;
  while ((read = getline(&line, &len, index_file)) != -1) {
    char* image_file = strtok(line, ":");

    if (i < SHOWN_LENGTH) {
      char* image_file_copy = (char *)calloc(strlen(image_file), sizeof(char));
      strcpy(image_file_copy, image_file);

      shown_files[i] = image_file_copy;
      shown_textures[i] = get_texture(image_file, imageSize);
    }

    i++;
  }
  fclose(index_file);
  free(line);

  /* Settings popup */
  int showSettings = false;

  /* load other things into memory */
  Font fontTtf = LoadFontEx("./assets/rubik/Rubik-Bold.ttf", 80, 0, 250);
  Image settingsImage = LoadImage("./assets/feather/settings.png");
  ImageResize(&settingsImage, 50, 50);
  Texture2D settingsTexture = LoadTextureFromImage(settingsImage);

  struct Vector2 imageSelectedVector = (Vector2){ 0, 0 };

  struct Vector2 mousePosition;
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    mousePosition = GetMousePosition();

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_K) || IsKeyPressed(KEY_W)) {
      if (imageSelectedVector.y > 0) {
        imageSelectedVector.y--;
      }
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_L) || IsKeyPressed(KEY_D)) {
      if(imageSelectedVector.x < gridColumns - 1) {
        imageSelectedVector.x++;
      } else if (imageSelectedVector.y != gridRows - 1) {
        imageSelectedVector.y++;
        imageSelectedVector.x = 0;
      }
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_J) || IsKeyPressed(KEY_S)) {
      if (imageSelectedVector.y < gridRows - 1) {
        imageSelectedVector.y++;
      }
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_A)) {
      if(imageSelectedVector.x > 0) {
        imageSelectedVector.x--;
      } else if (imageSelectedVector.y != 0) {
        imageSelectedVector.y--;
        imageSelectedVector.x = gridColumns - 1;
      }
    }

    if (IsKeyPressed(KEY_ENTER)) {
      // TODO: size
      char cmd[200];

      int index = (imageSelectedVector.y * gridColumns) + imageSelectedVector.x;
      char *name = shown_files[index];

      // ignore possibility of command injection
      sprintf(cmd, "xclip -selection clipboard -target image/png -i \"%s\"", name);
      if (system(cmd) != 0) {
        die(strerror(errno));
      }
    }

    /* heading */
    DrawTextEx(fontTtf, "Pick Sticker", (Vector2){ 0.0f, -5.0f }, (float)fontTtf.baseSize, 2, colorBlack);
    DrawRectangle(0, (float)fontTtf.baseSize - 10, 430, 4, colorBlack);
    DrawTexture(settingsTexture, windowWidth - 50, 0, colorWhite);

    /* selection rectangle */
    struct Rectangle selectionRectangle = (Rectangle){
      (gridStart.x + (imageSizeTotal * imageSelectedVector.x)),
      (gridStart.y + (imageSizeTotal * imageSelectedVector.y)),
      imageSizeTotal,
      imageSizeTotal
    };
    DrawRectangleRounded(selectionRectangle, 0.3f, 0, colorLightGray);
    DrawRectangleRoundedLines(selectionRectangle, 0.3f, 0, 1, colorDarkGray);

    for(int rowN = 0; rowN < gridRows; rowN++) {
      for(int columnN = 0; columnN < gridColumns; columnN++) {
        int index = (rowN * gridColumns) + columnN;
        int posX = gridStart.x + (imageSizeTotal * columnN) + imageMargin;
        int posY = gridStart.y + (imageSizeTotal * rowN) + imageMargin;
        DrawTexture(shown_textures[index], posX, posY, colorWhite);
      }
    }

    // if (CheckCollisionPointRec(mousePosition, ))
    // if(showSettings) {

    // }

    EndDrawing();
  }

  for(int i = 0; i < (sizeof(shown_files) / sizeof(shown_files[0])); ++i) {
    free(shown_files[i]);
  }

  UnloadFont(fontTtf);
  CloseWindow();

  return EXIT_SUCCESS;
}
