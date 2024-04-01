#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autocomplete.h"

int cmp_terms(const void* a, const void* b) {
  term* term_a = (term*)a;
  term* term_b = (term*)b;
  return strcmp(term_a->term, term_b->term);
}

void read_in_terms(term **terms, int *pnterms, char *filename) {
  FILE* fp = fopen(filename, "r");
  
  fscanf(fp, "%d", pnterms);

  *terms = (term*)malloc((*pnterms) * sizeof(term)); //Creating an array of term structs with *pnterms elements

  for (int i = 0; i < *pnterms; i++) {
    fscanf(fp, "%lf %[^\n]\n", &(*terms)[i].weight, (*terms)[i].term); //Assigning term and weight to each term struct element of terms 
  }

  qsort(*terms, *pnterms, sizeof(term), cmp_terms);

  fclose(fp);
}


/*Returns the index in terms of the 1st term in lexicographic ordering that matches the string "substr"*/

int lowest_match(struct term *terms, int nterms, char *substr){
  int first = 0;
  int last = nterms - 1;
  int length = strlen(substr);
  
  while (last >= first){
    int mid = (last + first)/2;
    int cmp = strncmp(terms[mid].term, substr, length);
    if (cmp != 0){
      if (cmp < 0){
        first = mid + 1;
      }
      else if (cmp > 0){
        last = mid - 1;
      }
    }
    else if (cmp == 0){
      int cmp_left = strncmp(terms[mid - 1].term, substr, length);
      if (cmp_left != 0 || mid == first){
        return mid;
      } else {
        return lowest_match(terms, mid-1, substr);
      }
    }
  } 
  return -1;
}

int highest_match(struct term *terms, int nterms, char *substr){
  int first = 0;
  int last = nterms - 1;
  int length = strlen(substr);

  while (last >= first) {
    int mid = (last + first)/2;
    int cmp = strncmp(terms[mid].term, substr, length);
    if (cmp != 0) {
      if (cmp < 0) {
        first = mid + 1;
      } else if (cmp > 0) {
        last = mid - 1;
      }
    } else if (cmp == 0) {
      while (first <= last) {
        int mid_right = (last + first)/2;
        int cmp_right = strncmp(terms[mid_right].term, substr, length);
        if (cmp_right == 0) {
          first = mid_right + 1;
        } else {
          last = mid_right - 1;
        }
      }
      return last;
    }
  } 
  return -1;
}

int cmp_weights(const void* a, const void* b) {
  term* term_a = (term*)a;
  term* term_b = (term*)b;
  if (term_a->weight > term_b->weight) {
    return -1;
  } else if (term_a->weight < term_b->weight) {
    return 1;
  } else {
    return 0;
  }
}

void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr) {
  int low = lowest_match(terms, nterms, substr);
  int high = highest_match(terms,nterms, substr);
  
  *n_answer = high - low + 1;
  *answer = (term*)malloc((*n_answer) * sizeof(term));

  for (int i = low; i <= high; i++) {
    (*answer)[i - low] = terms[i];
  }

  qsort(*answer, *n_answer - 1, sizeof(term), cmp_weights);
}