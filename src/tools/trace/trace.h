#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

namespace trace
{
    static const int FIELDNAME_LENGTH = 255;
    static const char *FEATURE_STRING = "TRACEDB";//the length of this string must be 7

    #pragma pack(1)
    typedef struct fieldinfo
    {
        char name[FIELDNAME_LENGTH + 1];
        uint32_t offset;
        uint32_t size;
        uint32_t element_bitsize;
        uint32_t element_size;
        uint32_t element_num;
    }fieldinfo_t;

    typedef struct headerinfo
    {
        char featurestr[8];
        uint32_t fieldinfo_num;
        uint32_t input_fieldinfo_num;
        uint32_t output_fieldinfo_num;
        uint32_t status_fieldinfo_num;
        uint32_t data_offset;
        uint32_t row_size;
    }headerinfo_t;
    #pragma pack()

    enum class domain_t : int
    {
        input,
        output,
        status
    };

    class trace_database
    {
        private:
            std::ofstream db;
            bool _is_enabled;
            bool _is_tracing;
            bool _is_open;
            bool _metainfo_ok;
            std::unordered_map<std::string, void *> input_bind_list;
            std::unordered_map<std::string, void *> output_bind_list;
            std::unordered_map<std::string, void *> status_bind_list;
            std::vector<fieldinfo_t> input_fieldinfo_list;
            std::vector<fieldinfo_t> output_fieldinfo_list;
            std::vector<fieldinfo_t> status_fieldinfo_list;
            std::unordered_map<std::string, int> input_fieldid_map;
            std::unordered_map<std::string, int> output_fieldid_map;
            std::unordered_map<std::string, int> status_fieldid_map;
            headerinfo_t hinfo;
            char *row_buffer;

            std::unordered_map<std::string, int> &get_fieldid_map(domain_t domain);
            std::unordered_map<std::string, void *> &get_bind_list(domain_t domain);
            std::vector<fieldinfo_t> &get_fieldinfo_list(domain_t domain);

            static std::vector<trace_database *> tdb_obj_list;

        public:
            trace_database(bool enable);
            ~trace_database();
            void create(std::string filename);
            bool is_open();
            void close();
            void clear_bind_list();
            void trace_on();
            void trace_off();
            void trace_set(bool on);
            bool is_tracing();
            void mark_signal(domain_t domain, std::string name, uint32_t element_size, uint32_t element_num);
            void mark_signal_bitmap(domain_t domain, std::string name, uint32_t bit_num, uint32_t element_num);
            void bind_signal(domain_t domain, std::string name, void *buf, uint32_t element_size, uint32_t element_num);
            void write_metainfo();
            void capture_input();
            void capture_output();
            void capture_status();
            void capture_output_status();
            void update_signal_raw(domain_t domain, std::string name, void *buf, uint32_t index);
            template <typename T> void update_signal(domain_t domain, std::string name, T value, uint32_t index);
            template <typename T> void update_signal_bit(domain_t domain, std::string name, uint64_t value, uint32_t bit_index, uint32_t index);
            void update_signal_bitmap(domain_t domain, std::string name, uint8_t value, uint32_t bit_index, uint32_t index);
            void update_signal_bitmap(domain_t domain, std::string name, void *buf, uint32_t index);
            void update_signal_bitmap_all(domain_t domain, std::string name, uint8_t value, uint32_t index);
            void write_row();
            void flush();
            static void flush_all_tdb();
    };

    template void trace_database::update_signal<>(domain_t domain, std::string name, uint8_t value, uint32_t index);
    template void trace_database::update_signal<>(domain_t domain, std::string name, uint16_t value, uint32_t index);
    template void trace_database::update_signal<>(domain_t domain, std::string name, uint32_t value, uint32_t index);
    template void trace_database::update_signal<>(domain_t domain, std::string name, uint64_t value, uint32_t index);
    template void trace_database::update_signal_bit<uint8_t>(domain_t domain, std::string name, uint64_t value, uint32_t bit_index, uint32_t index);
    template void trace_database::update_signal_bit<uint16_t>(domain_t domain, std::string name, uint64_t value, uint32_t bit_index, uint32_t index);
    template void trace_database::update_signal_bit<uint32_t>(domain_t domain, std::string name, uint64_t value, uint32_t bit_index, uint32_t index);
    template void trace_database::update_signal_bit<uint64_t>(domain_t domain, std::string name, uint64_t value, uint32_t bit_index, uint32_t index);
}