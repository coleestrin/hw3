#include<iostream>
#include<fstream>
#include<cstdlib>
#include<set>

using namespace std;

class Entry {
public:
  Entry();
  ~Entry();
  void display(ofstream& outfile);

  void set_tag(int _tag) { tag = _tag; }
  int get_tag() { return tag; }

  void set_valid(bool _valid) { valid = _valid; }
  bool get_valid() { return valid; }

  void set_ref(int _ref) { ref = _ref; }
  int get_ref() { return ref; }

private:  
  bool valid;
  unsigned tag;
  int ref;
};

Entry::Entry() {
  valid = false;
  tag = 0;
  ref = 0;
}

Entry::~Entry() {}

void Entry::display(ofstream& outfile) {
  outfile << "V: " << valid << " Tag: " << tag << endl;
}

class Cache {
public:
  Cache(int, int, int);
  ~Cache();

  void display(ofstream& outfile);

  int get_block_addr(unsigned long addr);
  int get_index(unsigned long addr);
  int get_tag(unsigned long addr);

  unsigned long retrieve_addr(int way, int index);

  bool hit(unsigned long addr);
  void access(unsigned long addr);

private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  int block_size;
  Entry **entries;
  int *next_evict;  // FIFO pointer per set
};

Cache::Cache(int _num_entries, int _assoc, int _block_size) {
  num_entries = _num_entries;
  assoc = _assoc;
  num_sets = num_entries / assoc;
  block_size = _block_size;

  entries = new Entry*[assoc];
  for (int i = 0; i < assoc; i++)
    entries[i] = new Entry[num_sets];

  next_evict = new int[num_sets];
  for (int i = 0; i < num_sets; i++)
    next_evict[i] = 0;
}

Cache::~Cache() {
  for (int i = 0; i < assoc; i++)
    delete[] entries[i];
  delete[] entries;
  delete[] next_evict;
}

void Cache::display(ofstream& outfile) {
  for (int i = 0; i < assoc; i++)
    for (int j = 0; j < num_sets; j++) {
      outfile << "Way " << i << " Set " << j << " ";
      entries[i][j].display(outfile);
    }
}

int Cache::get_block_addr(unsigned long addr) {
  return addr / block_size;
}

int Cache::get_index(unsigned long addr) {
  return get_block_addr(addr) % num_sets;
}

int Cache::get_tag(unsigned long addr) {
  return get_block_addr(addr) / num_sets;
}

unsigned long Cache::retrieve_addr(int way, int index) {
  return entries[way][index].get_tag() * num_sets + index;
}

bool Cache::hit(unsigned long addr) {
  int index = get_index(addr);
  int tag = get_tag(addr);
  for (int i = 0; i < assoc; i++)
    if (entries[i][index].get_valid() && entries[i][index].get_tag() == tag)
      return true;
  return false;
}

// FIFO: on miss, fill empty slot or overwrite next_evict slot
void Cache::access(unsigned long addr) {
  int index = get_index(addr);
  int tag = get_tag(addr);

  // If already in cache, do nothing (FIFO doesn't update on hit)
  for (int i = 0; i < assoc; i++)
    if (entries[i][index].get_valid() && entries[i][index].get_tag() == tag)
      return;

  // Look for empty slot
  for (int i = 0; i < assoc; i++) {
    if (!entries[i][index].get_valid()) {
      entries[i][index].set_valid(true);
      entries[i][index].set_tag(tag);
      return;
    }
  }

  // Set full: evict at FIFO pointer and advance
  int way = next_evict[index];
  entries[way][index].set_tag(tag);
  next_evict[index] = (way + 1) % assoc;
}

int main(int argc, char* argv[]) {

  /* Usage:
     Base:       ./cache_sim num_entries assoc input_file
     + blocks:   ./cache_sim num_entries assoc input_file block_size
     + L2:       ./cache_sim num_entries assoc input_file block_size L2_entries L2_assoc
  */
  if (argc < 4) {
    cout << "Usage: " << endl;
    cout << "  ./cache_sim num_entries associativity filename [block_size [L2_entries L2_assoc]]" << endl;
    return 0;
  }

  unsigned l1_entries = atoi(argv[1]);
  unsigned l1_assoc   = atoi(argv[2]);
  string input_filename = argv[3];

  int block_size = 1;
  if (argc >= 5)
    block_size = atoi(argv[4]);

  bool has_l2 = false;
  unsigned l2_entries = 0;
  unsigned l2_assoc = 0;
  if (argc >= 7) {
    has_l2 = true;
    l2_entries = atoi(argv[5]);
    l2_assoc   = atoi(argv[6]);
  }

  Cache l1(l1_entries, l1_assoc, block_size);

  Cache* l2 = NULL;
  if (has_l2)
    l2 = new Cache(l2_entries, l2_assoc, block_size);

  // Fully-associative shadow cache (same size as L1) for miss classification
  Cache fa_shadow(l1_entries, l1_entries, block_size);

  // Track all block addresses seen so far for compulsory detection
  set<int> seen_blocks;

  ifstream input;
  input.open(input_filename);
  if (!input.is_open()) {
    cerr << "Could not open input file " << input_filename << ". Exiting ..." << endl;
    exit(0);
  }

  ofstream output;
  output.open("cache_sim_output");

  unsigned long addr;
  while (input >> addr) {
    int block_addr = addr / block_size;

    bool l1_hit = l1.hit(addr);
    bool fa_hit = fa_shadow.hit(addr);
    bool is_new = (seen_blocks.find(block_addr) == seen_blocks.end());

    l1.access(addr);
    fa_shadow.access(addr);
    seen_blocks.insert(block_addr);

    if (l1_hit) {
      output << addr << " : HIT" << endl;
    } else {
      // Classify miss
      string miss_type;
      if (is_new)
        miss_type = "compulsory";
      else if (!fa_hit)
        miss_type = "capacity";
      else
        miss_type = "conflict";

      if (has_l2) {
        bool l2_hit = l2->hit(addr);
        l2->access(addr);
        output << addr << " : MISS " << miss_type
               << " (L2 " << (l2_hit ? "HIT" : "MISS") << ")" << endl;
      } else {
        output << addr << " : MISS " << miss_type << endl;
      }
    }
  }

  input.close();
  output.close();

  if (l2 != NULL)
    delete l2;

  return 0;
}