#include <stdio.h>

#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include  <errno.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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

static inline void draw_tab_button() {

}

[[gnu::pure]] static inline int get_columnN_from_mouse(Vector2 mousePosition, int imageSizeTotal) {
  return (int)mousePosition.x / imageSizeTotal;
}

[[gnu::pure]] static inline int get_rowN_from_mouse(Vector2 mousePosition, int imageSizeTotal, int menuBarHeight) {
  return ((int)mousePosition.y - menuBarHeight) / imageSizeTotal;
}

/**
 * @brief Determine if a string ends with a specific file extension
 */
static inline bool has_extension(const char *name, const char *ext)
{
  size_t name_length = strlen(name);
  size_t ext_length = strlen(ext);
  return name_length >= ext_length && !strcmp(name + name_length - ext_length, ext);
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
        strcat(s, "/");
        strcat(s, dir->d_name);

        DIR *subd = opendir(s);
        if (subd) {
          struct dirent *subdir;
          while((subdir = readdir(subd)) != NULL) {
            if (subdir->d_type == DT_REG) {

              char ss[PATH_MAX];
              strcpy(ss, s);
              strcat(ss, "/");
              strcat(ss, subdir->d_name);

              if (has_extension(ss, ".png")) {
                if (strstr(ss, "_small.png") == NULL) {
                  fprintf(index_file, "%s:\n", ss);
                }
              }
            }
          }
          closedir(subd);
        } else {
          die("Unexpected NULL pointer");
        }
      }
    }
    closedir(d);
  } else {
    die("Unexpected NULL pointer");
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
  create_index("./out/index_file", "./downloads");

  /* constants */
  const int imageSize = 100;
  const int imageMargin = 5;
  const int imageSizeTotal = imageSize + (imageMargin * 2);

  const int gridColumns = 9;
  const int gridRows = 7;

  const int menuBarHeight = 60;
  const int windowBorder = 5;
  const int windowWidth = (gridColumns * (imageSizeTotal)) + (windowBorder * 2);
  const int windowHeight = menuBarHeight + (gridRows * (imageSizeTotal)) + windowBorder;

  const Vector2 gridStart = (Vector2){ windowBorder, menuBarHeight - 5 };

  const Color colorBlack = (Color){ 52, 58, 64, 255 };
  const Color colorLightGray = (Color){ 233, 236, 239, 255 };
  const Color colorDarkGray = (Color){ 173, 181, 189, 255 };
  const Color colorWhite = WHITE;

  /* start */
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(windowWidth, windowHeight, "Pick Sticker");

  /* load images and their attributes into memory */
  #define SHOWN_LENGTH 100
  char* shown_files[SHOWN_LENGTH];
  Texture2D shown_textures[SHOWN_LENGTH];
  FILE *index_file = fopen("./out/index_file", "r");
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int i = 0;
  while ((read = getline(&line, &len, index_file)) != -1) {
    char* image_file = strtok(line, ":");
    if (image_file == NULL) {
      continue;
    }

    if (i < SHOWN_LENGTH) {
      char* image_file_copy = (char *)calloc(strlen(image_file)+1, sizeof(char));
      strcpy(image_file_copy, image_file);

      shown_files[i] = image_file_copy;
      shown_textures[i] = get_texture(image_file, imageSize);
    }

    i++;
  }
  fclose(index_file);
  free(line);

  /* all variables */
  Vector2 mousePosition, mouseDelta;
  int currentTab = 0;
  enum tabs { TAB_PICKER, TAB_SETTINGS };


  /* for title bar */
  Font fontTtf = LoadFontEx("./assets/rubik/Rubik-Bold.ttf", menuBarHeight, NULL, 0);
  Font smallFontTtf = LoadFontEx("./assets/rubik/Rubik-Medium.ttf", 30, NULL, 0);
  Image settingsImage = LoadImage("./assets/feather/settings.png");
  ImageResize(&settingsImage, 50, 50);
  Texture2D settingsTexture = LoadTextureFromImage(settingsImage);

  /* for image tab */
  Vector2 imageSelectedVector = (Vector2){ 0, 0 };

  /* misc */
  GuiSetFont(smallFontTtf);
  char text[128] = "Text";

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    mousePosition = GetMousePosition();
    mouseDelta = GetMouseDelta();

    /* heading */
    char *titleText = "Pick Sticker";
    DrawTextEx(fontTtf, titleText, (Vector2){ 0.0f, -5.0F }, (float)fontTtf.baseSize, 0, colorBlack);
    DrawRectangle(
      windowBorder - 1,
      (float)fontTtf.baseSize - 14,
      MeasureTextEx(fontTtf, titleText, (float)fontTtf.baseSize, 0).x - 5, // TODO: cache
      4,
      colorBlack
    );
    if (currentTab == TAB_PICKER) {
      Rectangle inputRec = (Rectangle){
        MeasureTextEx(fontTtf, "Pick Sticker", (float)fontTtf.baseSize, 0).x + 10, // TODO: cache
        10,
        200,
        40
      };
      int value = 20;
      GuiTextBox(inputRec, text, 18, true);
    }
    Rectangle tabPickerRec = (Rectangle){
      windowWidth - 245, menuBarHeight - 47, 115, 35
    };
    Rectangle tabSettingsRec = (Rectangle){
      windowWidth - 125, menuBarHeight - 47, 120, 35
    };
    GuiButton(tabPickerRec, "Picker");
    GuiButton(tabSettingsRec, "Settings");

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (CheckCollisionPointRec(mousePosition, tabPickerRec)) {
        currentTab = TAB_PICKER;
      } else if (CheckCollisionPointRec(mousePosition, tabSettingsRec)) {
        currentTab = TAB_SETTINGS;
      }
    }

    /* tabs */
    #define IsKeyPressedWithAlt(_KEY) ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(_KEY))

    if (currentTab == TAB_PICKER) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressedWithAlt(KEY_K)) {
      if (imageSelectedVector.y > 0) {
        imageSelectedVector.y--;
      }
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedWithAlt(KEY_L)) {
      if (imageSelectedVector.x < gridColumns - 1) {
        imageSelectedVector.x++;
      } else if (imageSelectedVector.y != gridRows - 1) {
        imageSelectedVector.y++;
        imageSelectedVector.x = 0;
      }
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressedWithAlt(KEY_J)) {
      if (imageSelectedVector.y < gridRows - 1) {
        imageSelectedVector.y++;
      }
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedWithAlt(KEY_H)) {
      if(imageSelectedVector.x > 0) {
        imageSelectedVector.x--;
      } else if (imageSelectedVector.y != 0) {
        imageSelectedVector.y--;
        imageSelectedVector.x = gridColumns - 1;
      }
    }
    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      // TODO: size
      char cmd[200];

      int index = (imageSelectedVector.y * gridColumns) + imageSelectedVector.x;
      char *name = shown_files[index];

      // ignore possibility of command injection
      sprintf(cmd, "xclip -selection clipboard -target image/png -i \"%s\"", name);
      if (system(cmd) != 0) {
        die(strerror(errno));
      }
      exit(0);
    }

    /* selection rectangle */
    if (abs((int)mouseDelta.x) >= 1 || abs((int)mouseDelta.y) >= 1) {
      // columnN and rowN are indexed starting from zero
      int columnN = get_columnN_from_mouse(mousePosition, imageSizeTotal);
      int rowN = get_rowN_from_mouse(mousePosition, imageSizeTotal, menuBarHeight);
      if (mousePosition.y > menuBarHeight - 5) {
        if (columnN < gridColumns) {
          imageSelectedVector.x = columnN;
        }
        if (rowN < gridRows) {
          imageSelectedVector.y = rowN;
        }
      }
    }
    Rectangle selectionRectangle = (Rectangle){
      (gridStart.x + (imageSizeTotal * imageSelectedVector.x)),
      (gridStart.y + (imageSizeTotal * imageSelectedVector.y)),
      imageSizeTotal,
      imageSizeTotal
    };
    DrawRectangleRounded(selectionRectangle, 0.3f, 0, colorLightGray);
    DrawRectangleRoundedLines(selectionRectangle, 0.3f, 0, 1, colorDarkGray);

    /* actual images */
    for(int rowN = 0; rowN < gridRows; rowN++) {
      for(int columnN = 0; columnN < gridColumns; columnN++) {
        int index = (rowN * gridColumns) + columnN;
        int posX = gridStart.x + (imageSizeTotal * columnN) + imageMargin;
        int posY = gridStart.y + (imageSizeTotal * rowN) + imageMargin;
        DrawTexture(shown_textures[index], posX, posY, colorWhite);
      }
    }


    } else if (currentTab == TAB_SETTINGS) {

    }

    EndDrawing();
  }

  for (int i = 0; i < (sizeof(shown_files) / sizeof(shown_files[0])); ++i) {
    free(shown_files[i]);
  }

  UnloadFont(fontTtf);
  CloseWindow();

  return EXIT_SUCCESS;
}
