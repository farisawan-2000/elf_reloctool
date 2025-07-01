#include "elfio/elfio.hpp"
#include <iostream>
#include <unordered_map>
#include "mips_reloc_types.hpp"

using namespace ELFIO;

elfio reader;

std::unordered_map<std::string, uint32_t> section2idx;

extern std::unordered_map<int, std::string> reloctypes;

#define swap(x) (__builtin_bswap32((x)))

uint32_t sizeconv(uint32_t size) {
    return size * sizeof(uint32_t);
}

void populate_convtbl() {
    Elf_Half sec_num = reader.sections.size();
    for ( int i = 0; i < sec_num; ++i ) {
        const section* psec = reader.sections[i];
        section2idx[std::string(psec->get_name())] = i;
    }
}

void populate_symbols(std::vector<uint32_t> &relVec, const uint32_t *accessors, uint32_t relsize) {
    section *symtable = reader.sections[".symtab"];
    const symbol_section_accessor symbols( reader, symtable );
    for (uint i = 0; i < (relsize / 2 / 4); i++) {
        std::string name; Elf64_Addr value; Elf_Xword size;
        unsigned char bind; unsigned char type;
        Elf_Half section_index; unsigned char other;

        uint32_t symIdx = swap(accessors[(i * 2) + 1]) >> 8;
        uint32_t offset = swap(accessors[(i * 2) + 0]);

        symbols.get_symbol(symIdx, name, value, size, bind,
        type, section_index, other );

        if (type != R_MIPS_NONE) {
            std::cout << "Found " << name << " with reloc type " << reloctypes[type] << std::endl;
        }

        if (section_index == 0) {
            // No section, i.e. symbol is not in this file
        } else {
            // put it on the reloc table (and byteswap it back)
            // relVec.emplace_back(swap(offset));
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [input/output .elf/.o file]" << std::endl;
        return 1;
    }

    if (!reader.load(argv[1])) {
        std::cout << "Error: " << argv[1] << ": No such file." << std::endl;
        return 2;
    }

    populate_convtbl();

    Elf_Half sec_num = reader.sections.size();

    for (int i = 0; i < sec_num; i++) {
        const section *psec = reader.sections[i];

        // std::cout << "Reading section "
        //           << psec->get_name()
        //           << " ..." << std::endl;
        
        std::string relname;

        relname = std::string(".rel") + psec->get_name();

        // std::cout << "    rel name: " << relname << " ..." << std::endl;

        section *prel = reader.sections[relname];

        section *symtable = reader.sections[".symtab"];
        const symbol_section_accessor symbols( reader, symtable );

        if (prel) {
            const char *reldata = prel->get_data();
            const uint32_t *accessors = reinterpret_cast<const uint32_t*>(reldata);
            std::vector<uint32_t> relocs;

            relocation_section_accessor reloc_accessor(reader, prel);

            for (uint i = 0; i < reloc_accessor.get_entries_num(); ++i) {
                Elf64_Addr offset;
                Elf_Word symbol_index;
                unsigned int type;
                Elf_Sxword addend = 0;

                std::string name; Elf64_Addr value; Elf_Xword size;
                unsigned char bind; unsigned char type2;
                Elf_Half section_index; unsigned char other;

                reloc_accessor.get_entry((Elf_Xword)i, offset, symbol_index, type, addend);
                symbols.get_symbol(symbol_index, name, value, size, bind,
                type2, section_index, other );

                // get_entry(ELFIO::Elf_Xword, ELFIO::Elf64_Addr&, ELFIO::Elf_Word&, unsigned int&, ELFIO::Elf_Sxword&)
                // if (prel->get_type() == SHT_RELA) {
                // } else {
                //     reloc_accessor.get_entry((Elf_Xword)i, offset, symbol_index, type);
                // }

                // Print relocation type and symbol index
                if (bind == STB_LOCAL) {
                    std::cout << "Found " << name
                          << " @ 0x" << std::hex << offset << std::dec << ", type = " << reloctypes[type] << std::endl;
                }
            }

            // populate_symbols(relocs, accessors, prel->get_size());

            // std::string outname = std::string("out/") + prel->get_name();

            // std::cout << "Making " << outname << "..." << std::endl;

            // FILE *f = fopen(outname.c_str(), "wb+");
            // fwrite(prel->get_data(), 1, prel->get_size(), f);
            // fclose(f);
        }

    }


    // add our epic new section
    // section* out_sec = reader.sections.add(".reloc_data");
    // out_sec->set_type( SHT_PROGBITS );
    // out_sec->set_flags( SHF_ALLOC );
    // out_sec->set_addr_align( 0x10 );

    // out_sec->set_data((const char *)swappedOutput.data(), (Elf_Word) (swappedOutput.size() * sizeof(uint32_t)));
    // reader.save(argv[1]);

    return 0;
}
