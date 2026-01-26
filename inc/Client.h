/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omadali < omadali@student.42kocaeli.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 23:42:50 by sdaban            #+#    #+#             */
/*   Updated: 2026/01/26 22:21:55 by omadali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
#define CLIENT_H

#include <string>


class Client {
public:
    Client();
    Client(int fd, const ServerConfig* config);
    ~Client();

    int get_fd() const;
    const std::string& get_inbuf() const;
    const std::string& get_outbuf() const;
    bool is_header_done() const;
    bool should_close() const;
    time_t get_last_activity() const;
    const Request& get_request() const;
    const ServerConfig* get_config() const;

    void append_inbuf(const char* data, size_t len);
    void set_outbuf(const std::string& data);
    void consume_outbuf(size_t len);
    void set_header_done(bool done);
    void set_close_after_send(bool close);
    void update_activity();
    void set_request(const Request& req);
    void clear();

private:
    int m_fd;
    std::string m_inbuf;
    std::string m_outbuf;
    bool m_header_done;
    bool m_close_after_send;
    time_t m_last_activity;
    Request m_request;
    const ServerConfig* m_config;
};

#endif