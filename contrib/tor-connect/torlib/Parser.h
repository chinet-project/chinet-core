/*-
 * Copyright (c) 2021, Chinet project, https://chinet.io
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2016 Petr Benes https://github.com/wbenny/mini-tor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once
#include "OnionRouter.h"
#include "Util.h"

enum class entry_type
{
    entry_r = 'r',
    entry_a = 'a',
    entry_s = 's',
    entry_v = 'v',
    entry_w = 'w',
    entry_p = 'p',
};

enum class entry_r_type
{
    //
    // start counting from 1,
    // because there is the "r" control word
    // on the index 0.
    //
    entry_r_nickname = 1,
    entry_r_identity,
    entry_r_digest,
    entry_r_publication_date,
    entry_r_publication_time,
    entry_r_ip,
    entry_r_or_port,
    entry_r_dir_port,

    //
    // router_status_entry_r_item_count = 9
    //
    entry_r_item_count,
};

enum class control_words
{
    not_determined,
    onion_key,
    signing_key,
    begin_public_key,
    end_public_key,
    ntor_onion_key,
};



class Parser
{
private:
	const string preamble_control_words = "valid-until";
    
    map<control_words, string> list_control_words;    
    const string directory_footer_control_words = "directory-footer";
    const string accept_ports = "accept";
    const string reject_ports = "reject";
    bool PortSearch(string in_line, int port_search);
public:
    vector<string> ParsString(const string& in_str, string del);
    tm GetValidUntil(const string& in_str);    
    shared_ptr<OnionRouter> GetOnionRouter(vector<string>& in_data, bool random, int or_port, int dir_port, string sh_ip, vector<string> flags, int search_port = 0);
    bool SetOnionRouterKeys(shared_ptr<OnionRouter> onion_node, vector<string>& in_data);
    vector<string> SearchOnionRouter(vector<string>& in_data, bool random, int or_port, int dir_port, string sh_ip, vector<string> flags, int search_port = 0);
    Parser();
};

