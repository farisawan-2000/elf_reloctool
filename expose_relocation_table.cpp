#include "elfio/elfio.hpp"
#include <iostream>
#include <unordered_map>

using namespace ELFIO;

struct reloc_header {
    uint32_t geo_offset; uint32_t geo_size;
    uint32_t geo_reloc_offset; uint32_t geo_reloc_size;

    uint32_t model_offset; uint32_t model_size;
    uint32_t model_reloc_offset; uint32_t model_reloc_size;

    uint32_t anim_offset; uint32_t anim_size;
    uint32_t anim_reloc_offset; uint32_t anim_reloc_size;

    uint32_t collision_offset; uint32_t collision_size;
    uint32_t texture_offset; uint32_t texture_size;
};

enum RelocSection64 : uint32_t {
    RS_GEO = 0,
    RS_MODEL,
    RS_ANIM,
    RS_COLLISION,
    RS_TEXTURE,
};

elfio reader;
reloc_header header = { 0 };

std::unordered_map<std::string, uint32_t> section2idx;

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

        if (section_index == 0) {
            // No section, i.e. symbol is not in this file
        } else {
            // put it on the reloc table (and byteswap it back)
            relVec.emplace_back(swap(offset));
            if (section2idx[".rodata.collision"] == section_index) {
                relVec.emplace_back(swap(RS_COLLISION));
            } else if (section2idx[".rodata"] == section_index) {
                relVec.emplace_back(swap(RS_MODEL));
            } else if (section2idx[".rodata.geolayout"] == section_index) {
                relVec.emplace_back(swap(RS_GEO));
            } else if (section2idx[".rodata.animation"] == section_index) {
                relVec.emplace_back(swap(RS_ANIM));
            }
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

    // Need this to tell the game which section to use to point a reloc
    populate_convtbl();

    // LAYOUT ORDER:
    //  header
    //  relocs
    //    geolayout
    //    model
    //    anim
    //  data
    //    .rodata.geolayout (the important part)
    //    .rodata (models)
    //    .rodata.animation (anims)
    //    .rodata.collision (at the end)
    //    .rodata.texture (no relocs and also ideally in their own FS soon:tm:)

    std::vector<uint32_t> geoRelocs;
    std::vector<uint32_t> modelRelocs;
    std::vector<uint32_t> animRelocs;

    const section *geolayout = reader.sections[".rodata.geolayout"];
    const section *rodata = reader.sections[".rodata"];
    const section *collision = reader.sections[".rodata.collision"];
    const section *animation = reader.sections[".rodata.animation"];
    const section *textureseg = reader.sections[".rodata.texture"];

    // TODO:
    //  we need to know the reloc sizes to be able to place file offsets

    // geo data
    if (geolayout) {
        header.geo_size = geolayout->get_size();

        const section *psec = reader.sections[".rel.rodata.geolayout"];
        if (psec) {
            const char *reldata = psec->get_data();
            const uint32_t *accessors = reinterpret_cast<const uint32_t*>(reldata);
            header.geo_reloc_offset = sizeof(reloc_header);
            populate_symbols(geoRelocs, accessors, psec->get_size());
            header.geo_reloc_size = sizeconv(geoRelocs.size());
        }
    }

    // model data
    if (rodata) {
        header.model_size = rodata->get_size();

        const section *psec = reader.sections[".rel.rodata"];
        if (psec) {
            const char *reldata = psec->get_data();
            const uint32_t *accessors = reinterpret_cast<const uint32_t*>(reldata);
            populate_symbols(modelRelocs, accessors, psec->get_size());
            header.model_reloc_offset = sizeof(reloc_header) + header.geo_reloc_size;
            header.model_reloc_size = sizeconv(modelRelocs.size());
        }
    }

    // animation data
    if (animation) {
        const section *psec = reader.sections[".rel.rodata.animation"];
        if (psec) {
            const char *reldata = psec->get_data();
            const uint32_t *accessors = reinterpret_cast<const uint32_t*>(reldata);

            populate_symbols(animRelocs, accessors, psec->get_size());
            header.anim_size = animation->get_size();
            header.anim_reloc_offset = sizeof(reloc_header) + header.geo_reloc_size + header.model_reloc_size;
            header.anim_reloc_size = sizeconv(animRelocs.size());
        }
    }

    // collision data
    if (collision) {
        header.collision_size = collision->get_size();
    } else {
        header.collision_offset = 0;
        header.collision_size = 0;
    }

    // texture data
    if (textureseg) {
        header.texture_size = textureseg->get_size();
    } else {
        header.texture_offset = 0;
        header.texture_size = 0;
    }

    uint32_t totalHeaderSize = sizeof(reloc_header)
                             + header.geo_reloc_size
                             + header.model_reloc_size
                             + header.anim_reloc_size;

    std::cout << "geo relocsize: " << header.geo_reloc_size <<" bytes." << std::endl;
    std::cout << "model relocsize: " << header.model_reloc_size <<" bytes." << std::endl;
    std::cout << "anim relocsize: " << header.anim_reloc_size <<" bytes." << std::endl;
    std::cout << "Total Headersize: " << totalHeaderSize <<" bytes." << std::endl;


    header.geo_offset = totalHeaderSize;
    header.model_offset = totalHeaderSize + header.geo_size;
    header.anim_offset = totalHeaderSize + header.geo_size + header.model_size;
    header.collision_offset = totalHeaderSize + header.geo_size + header.model_size + header.anim_size;
    header.texture_offset = totalHeaderSize + header.geo_size + header.model_size + header.anim_size + header.collision_size;

    // write the header but swap everything back to BE

    std::vector<uint32_t> swappedOutput;

    section* out_sec = reader.sections.add(".object_relocation");
    out_sec->set_type( SHT_PROGBITS );
    out_sec->set_flags( SHF_ALLOC );
    out_sec->set_addr_align( 0x10 );

    #define write_swapped(x) swappedOutput.emplace_back(swap(header.x))
    write_swapped(geo_offset); write_swapped(geo_size);
    write_swapped(geo_reloc_offset); write_swapped(geo_reloc_size);
    write_swapped(model_offset); write_swapped(model_size);
    write_swapped(model_reloc_offset); write_swapped(model_reloc_size);
    write_swapped(anim_offset); write_swapped(anim_size);
    write_swapped(anim_reloc_offset); write_swapped(anim_reloc_size);
    write_swapped(collision_offset); write_swapped(collision_size);
    write_swapped(texture_offset); write_swapped(texture_size);

    for (auto i : geoRelocs) {
        swappedOutput.emplace_back(i);
    }
    for (auto i : modelRelocs) {
        swappedOutput.emplace_back(i);
    }
    for (auto i : animRelocs) {
        swappedOutput.emplace_back(i);
    }

    // for (int i = 0; i < swappedOutput.size(); i++) {
    //     if (i % 4 == 0) {
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::hex << swappedOutput[i] << " ";
    // }
    // std::cout << std::endl;

    out_sec->set_data((const char *)swappedOutput.data(), (Elf_Word) (swappedOutput.size() * sizeof(uint32_t)));
    reader.save(argv[1]);

    return 0;
}
