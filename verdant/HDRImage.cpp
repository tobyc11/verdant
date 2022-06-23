// HDR run-length encoding should not be rewritten on our own
// We take from the official implementation at ray/src/common/color.c

#include "HDRImage.h"
#include "MathDefs.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>

#define BUF_LEN 256

#define MINELEN 8
#define MAXELEN 0x7fff

#define RED 0
#define GRN 1
#define BLU 2
#define EXP 3
#define COLXS 128

#define copycolr(c1, c2)                                                       \
  (c1[0] = c2[0], c1[1] = c2[1], c1[2] = c2[2], c1[3] = c2[3])

typedef unsigned char COLR[4];

namespace {
int fgetline(char *buf, size_t buf_len, FILE *f) {
  int i = 0;
  while (i < BUF_LEN) {
    int ch = fgetc(f);
    if (ch == '\n') {
      buf[i++] = 0;
      break;
    } else {
      buf[i++] = ch;
    }
  }
  return i - 1;
}

int oldreadcolrs(COLR *scanline, int len, FILE *fp) {
  int rshift = 0;
  int i;

  while (len > 0) {
    scanline[0][RED] = getc(fp);
    scanline[0][GRN] = getc(fp);
    scanline[0][BLU] = getc(fp);
    scanline[0][EXP] = i = getc(fp);
    if (i == EOF)
      return (-1);
    if (scanline[0][GRN] == 1 &&
        (scanline[0][RED] == 1) & (scanline[0][BLU] == 1)) {
      i = scanline[0][EXP] << rshift;
      while (i--) {
        copycolr(scanline[0], scanline[-1]);
        if (--len <= 0)
          return (0);
        scanline++;
      }
      rshift += 8;
    } else {
      scanline++;
      len--;
      rshift = 0;
    }
  }
  return (0);
}

int freadcolrs(COLR *scanline, int len, FILE *fp) {
  int i, j;
  int code, val;
  /* determine scanline type */
  if ((len < MINELEN) | (len > MAXELEN))
    return (oldreadcolrs(scanline, len, fp));
  if ((i = getc(fp)) == EOF)
    return (-1);
  if (i != 2) {
    ungetc(i, fp);
    return (oldreadcolrs(scanline, len, fp));
  }
  scanline[0][GRN] = getc(fp);
  scanline[0][BLU] = getc(fp);
  if ((i = getc(fp)) == EOF)
    return (-1);
  if ((scanline[0][GRN] != 2) | ((scanline[0][BLU] & 0x80) != 0)) {
    scanline[0][RED] = 2;
    scanline[0][EXP] = i;
    return (oldreadcolrs(scanline + 1, len - 1, fp));
  }
  if ((scanline[0][BLU] << 8 | i) != len)
    return (-1); /* length mismatch! */
                 /* read each component */
  for (i = 0; i < 4; i++)
    for (j = 0; j < len;) {
      if ((code = getc(fp)) == EOF)
        return (-1);
      if (code > 128) { /* run */
        code &= 127;
        if ((val = getc(fp)) == EOF)
          return -1;
        if (j + code > len)
          return -1; /* overrun */
        while (code--)
          scanline[j++][i] = val;
      } else { /* non-run */
        if (j + code > len)
          return -1; /* overrun */
        while (code--) {
          if ((val = getc(fp)) == EOF)
            return -1;
          scanline[j++][i] = val;
        }
      }
    }
  return (0);
}

verdant::float3 colr_to_float3(COLR clr) {
  verdant::float3 col;
  double f;

  if (clr[EXP] == 0)
    col[RED] = col[GRN] = col[BLU] = 0.0;
  else {
    f = ldexp(1.0, (int)clr[EXP] - (COLXS + 8));
    col[RED] = (clr[RED] + 0.5) * f;
    col[GRN] = (clr[GRN] + 0.5) * f;
    col[BLU] = (clr[BLU] + 0.5) * f;
  }
  return col;
}
} // namespace

namespace verdant {
HDRImage::HDRImage(const std::string &file_name) : valid(false) {
  char buf[BUF_LEN];
  FILE *f = fopen(file_name.c_str(), "rb");
  if (!f) {
    return;
  }

  // Check signature
  fread(buf, 11, 1, f);
  if (memcmp(buf, "#?RADIANCE\n", 10)) {
    fclose(f);
    return;
  }

  int line_len;
  do {
    line_len = fgetline(buf, BUF_LEN, f);
  } while (line_len > 0);

  line_len = fgetline(buf, BUF_LEN, f);
  sscanf(buf, "-Y %d +X %d", &height, &width);

  s = std::make_unique<float3[]>(width * height);

  auto colr_buffer = std::make_unique<COLR[]>(width);
  int y, x;
  int i = 0;
  for (y = 0; y < height; y++) {
    if (freadcolrs(colr_buffer.get(), width, f) < 0) {
      fclose(f);
      return;
    }

    for (x = 0; x < width; x++) {
      float3 color = colr_to_float3(colr_buffer[x]);
      s[i++] = color;
    }
  }

  fclose(f);
  valid = true;
}
} // namespace verdant
