#include "trace.h"

namespace trace
{
    std::vector<trace_database *> trace_database::tdb_obj_list;

    std::unordered_map<std::string, int> &trace_database::get_fieldid_map(domain_t domain)
    {
        switch(domain)
        {
            case domain_t::input:
                return input_fieldid_map;
            case domain_t::output:
                return output_fieldid_map;
            case domain_t::status:
                return status_fieldid_map;
            default:
                assert(false);
                return input_fieldid_map;
        }
    }

    std::unordered_map<std::string, void *> &trace_database::get_bind_list(domain_t domain)
    {
        switch(domain)
        {
            case domain_t::input:
                return input_bind_list;
            case domain_t::output:
                return output_bind_list;
            case domain_t::status:
                return status_bind_list;
            default:
                assert(false);
                return input_bind_list;
        }
    }

    std::vector<fieldinfo_t> &trace_database::get_fieldinfo_list(domain_t domain)
    {
        switch(domain)
        {
            case domain_t::input:
                return input_fieldinfo_list;
            case domain_t::output:
                return output_fieldinfo_list;
            case domain_t::status:
                return status_fieldinfo_list;
            default:
                assert(false);
                return input_fieldinfo_list;
        }
    }

    trace_database::trace_database(bool enable) : hinfo({})
    {
        _is_enabled = enable;
        _is_tracing = false;
        _is_open = false;
        _metainfo_ok = false;
        row_buffer = NULL;
        tdb_obj_list.push_back(this);
    }

    trace_database::~trace_database()
    {
        if(row_buffer)
        {
            delete[] row_buffer;
            row_buffer = NULL;
        }

        for(auto iter = tdb_obj_list.begin();iter != tdb_obj_list.end();iter++)
        {
            if(*iter == this)
            {
                tdb_obj_list.erase(iter);
                break;
            }
        }
    }

    void trace_database::create(std::string filename)
    {
        if(_is_enabled)
        {
            if(db.is_open())
            {
                db.close();
            }

            db.open(filename, std::ofstream::binary);
            _is_open = db.is_open();
            _is_tracing = false;
            _metainfo_ok = false;
            assert(_is_open);
        }
    }

    bool trace_database::is_open()
    {
        return _is_open;
    }

    void trace_database::close()
    {
        if(_is_enabled)
        {
            if(db.is_open())
            {
                db.close();
            }

            if(row_buffer)
            {
                delete[] row_buffer;
                row_buffer = NULL;
            }

            _is_open = false;
            _is_tracing = false;
            _metainfo_ok = false;
        }
    }

    void trace_database::clear_bind_list()
    {
        if(_is_enabled)
        {
            assert(_is_open && !_metainfo_ok);
            input_bind_list.clear();
            output_bind_list.clear();
            status_bind_list.clear();
            input_fieldinfo_list.clear();
            output_fieldinfo_list.clear();
            status_fieldinfo_list.clear();
            input_fieldid_map.clear();
            output_fieldid_map.clear();
            status_fieldid_map.clear();
        }
    }

    void trace_database::trace_on()
    {
        if(_is_enabled)
        {
            assert(_is_open && _metainfo_ok);
            _is_tracing = true;
        }
    }

    void trace_database::trace_off()
    {
        _is_tracing = false;
    }

    void trace_database::trace_set(bool on)
    {
        if(on)
        {
            trace_on();
        }
        else
        {
            trace_off();
        }
    }

    bool trace_database::is_tracing()
    {
        return _is_tracing;
    }

    void trace_database::mark_signal(domain_t domain, std::string name, uint32_t element_size, uint32_t element_num)
    {
        if(_is_enabled)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) == bind_list.end());
            assert(name.length() <= FIELDNAME_LENGTH);
            bind_list[name] = NULL;
            fieldinfo_t t_info;
            memset(&t_info, 0, sizeof(t_info));
            memcpy(t_info.name, name.c_str(), name.length());
            t_info.size = element_size * element_num;
            t_info.element_bitsize = element_num * 8;
            t_info.element_num = element_num;
            t_info.element_size = element_size;
            fieldid_map[name] = (int)fieldinfo_list.size();
            fieldinfo_list.push_back(t_info);
        }
    }

    void trace_database::mark_signal_bitmap(domain_t domain, std::string name, uint32_t bit_num, uint32_t element_num)
    {
        if(_is_enabled)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) == bind_list.end());
            assert(name.length() <= FIELDNAME_LENGTH);
            bind_list[name] = NULL;
            fieldinfo_t t_info;
            memset(&t_info, 0, sizeof(t_info));
            memcpy(t_info.name, name.c_str(), name.length());
            uint32_t bitmap_unit_size = sizeof(uint8_t);
            uint32_t bitmap_unit_bitsize = sizeof(uint8_t) * 8;
            uint32_t element_size = ((bit_num + bitmap_unit_bitsize - 1) / bitmap_unit_bitsize) * bitmap_unit_size;
            t_info.size = element_size * element_num;
            t_info.element_bitsize = bit_num;
            t_info.element_num = element_num;
            t_info.element_size = element_size;
            fieldid_map[name] = (int)fieldinfo_list.size();
            fieldinfo_list.push_back(t_info);
        }
    }

    void trace_database::bind_signal(domain_t domain, std::string name, void *buf, uint32_t element_size, uint32_t element_num)
    {
        if(_is_enabled)
        {
            auto &bind_list = get_bind_list(domain);
            mark_signal(domain, name, element_size, element_num);
            bind_list[name] = buf;
        }
    }

    void trace_database::write_metainfo()
    {
        if(_is_enabled)
        {
            assert(!_metainfo_ok);
            memset(&hinfo, 0, sizeof(hinfo));
            memcpy(hinfo.featurestr, FEATURE_STRING, strlen(FEATURE_STRING));
            hinfo.fieldinfo_num = (uint32_t)(input_fieldinfo_list.size() + output_fieldinfo_list.size() + status_fieldinfo_list.size());
            hinfo.input_fieldinfo_num = (uint32_t)input_fieldinfo_list.size();
            hinfo.output_fieldinfo_num = (uint32_t)output_fieldinfo_list.size();
            hinfo.status_fieldinfo_num = (uint32_t)status_fieldinfo_list.size();
            hinfo.data_offset = sizeof(hinfo) + sizeof(fieldinfo_t) * hinfo.fieldinfo_num;
            hinfo.row_size = 0;

            for(auto &item : input_fieldinfo_list)
            {
                item.offset = hinfo.row_size;
                hinfo.row_size += item.size;
            }

            for(auto &item : output_fieldinfo_list)
            {
                item.offset = hinfo.row_size;
                hinfo.row_size += item.size;
            }

            for(auto &item : status_fieldinfo_list)
            {
                item.offset = hinfo.row_size;
                hinfo.row_size += item.size;
            }

            row_buffer = new char[hinfo.row_size];
            assert(row_buffer);
            memset(row_buffer, 0, hinfo.row_size);
            db.write((const char *)&hinfo, sizeof(hinfo));

            for(auto const &item : input_fieldinfo_list)
            {
                db.write((const char *)&item, sizeof(item));
            }

            for(auto const &item : output_fieldinfo_list)
            {
                db.write((const char *)&item, sizeof(item));
            }

            for(auto const &item : status_fieldinfo_list)
            {
                db.write((const char *)&item, sizeof(item));
            }

            db.flush();
            db.seekp(hinfo.data_offset, std::ios::beg);
            _metainfo_ok = true;
        }
    }

    void trace_database::capture_input()
    {
        if(_is_tracing)
        {
            for(auto item : input_fieldinfo_list)
            {
                if(input_bind_list[item.name])
                {
                    memcpy(row_buffer + item.offset, input_bind_list[item.name], item.size);
                }
            }
        }
    }

    void trace_database::capture_output()
    {
        if(_is_tracing)
        {
            for(auto const &item : output_fieldinfo_list)
            {
                if(output_bind_list[item.name])
                {
                    memcpy(row_buffer + item.offset, output_bind_list[item.name], item.size);
                }
            }
        }
    }

    void trace_database::capture_status()
    {
        if(_is_tracing)
        {
            for(auto const &item : status_fieldinfo_list)
            {
                if(status_bind_list[item.name])
                {
                    memcpy(row_buffer + item.offset, status_bind_list[item.name], item.size);
                }
            }
        }
    }

    void trace_database::capture_output_status()
    {
        capture_output();
        capture_status();
    }

    void trace_database::update_signal_raw(domain_t domain, std::string name, void *buf, uint32_t index)
    {
        if(_is_tracing)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) != bind_list.end());
            assert(!bind_list[name]);
            auto const &item = fieldinfo_list[fieldid_map[name]];
            assert(index < item.element_num);
            memcpy(row_buffer + item.offset + item.element_size * index, buf, item.element_size);
        }
    }

    template <typename T> void trace_database::update_signal(domain_t domain, std::string name, T value, uint32_t index)
    {
        if(_is_tracing)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) != bind_list.end());
            auto const &item = fieldinfo_list[fieldid_map[name]];
            assert(item.element_size == sizeof(T));
            update_signal_raw(domain, name, (void *)&value, index);
        }
    }

    template <typename T> void trace_database::update_signal_bit(domain_t domain, std::string name, uint64_t value, uint32_t bit_index, uint32_t index)
    {
        if(_is_tracing)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) != bind_list.end());
            assert(!bind_list[name]);
            auto const &item = fieldinfo_list[fieldid_map[name]];
            assert(index < item.element_num);
            assert(item.element_size == sizeof(T));
            assert(bit_index < item.element_bitsize);
            T *stored_value = (T *)(row_buffer + item.offset + item.element_size * index);
            *stored_value = (*stored_value & (~(1 << bit_index))) | ((value & 0x01) << bit_index);
        }
    }

    void trace_database::update_signal_bitmap(domain_t domain, std::string name, uint8_t value, uint32_t bit_index, uint32_t index)
    {
        if(_is_tracing)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) != bind_list.end());
            assert(!bind_list[name]);
            auto const &item = fieldinfo_list[fieldid_map[name]];
            assert(index < item.element_num);
            assert(bit_index < item.element_bitsize);
            uint32_t group_index = bit_index / (sizeof(uint8_t) * 8);
            uint32_t group_bit_index = bit_index % (sizeof(uint8_t) * 8);
            uint8_t *stored_value = (uint8_t *)(row_buffer + item.offset + item.element_size * index + sizeof(uint8_t) * group_index);
            *stored_value = (*stored_value & (~(1 << group_bit_index))) | ((value & 0x01) << group_bit_index);
        }
    }

    void trace_database::update_signal_bitmap(domain_t domain, std::string name, void *buf, uint32_t index)
    {
        if(_is_tracing)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) != bind_list.end());
            assert(!bind_list[name]);
            auto const &item = fieldinfo_list[fieldid_map[name]];
            assert(index < item.element_num);
            memcpy(row_buffer + item.offset + item.element_size * index, buf, item.element_size);
        }
    }

    void trace_database::update_signal_bitmap_all(domain_t domain, std::string name, uint8_t value, uint32_t index)
    {
        if(_is_tracing)
        {
            auto &fieldid_map = get_fieldid_map(domain);
            auto &bind_list = get_bind_list(domain);
            auto &fieldinfo_list = get_fieldinfo_list(domain);
            assert(bind_list.find(name) != bind_list.end());
            assert(!bind_list[name]);
            auto const &item = fieldinfo_list[fieldid_map[name]];
            assert(index < item.element_num);
            memset(row_buffer + item.offset + item.element_size * index, value ? 0xff : 0, item.element_size);
        }
    }

    void trace_database::write_row()
    {
        if(_is_enabled)
        {
            assert(_metainfo_ok);

            if(_is_tracing)
            {
                db.write(row_buffer, hinfo.row_size);
                db.flush();
                memset(row_buffer, 0, hinfo.row_size);
            }
        }
    }

    void trace_database::flush()
    {
        db.flush();
    }

    void trace_database::flush_all_tdb()
    {
        for(auto iter : tdb_obj_list)
        {
            iter->flush();
        }
    }
}