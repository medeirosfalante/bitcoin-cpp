#include <bitcoin/system.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <list>
#include <iterator>

using namespace bc;

int main()
{
    //declando um vetor de 16 bytes
    data_chunk seedChunk(16);
    // preencher o o vetor com a seed
    pseudo_random_fill(seedChunk);
    // criando um mnemonico
    wallet::word_list writtenWord = wallet::create_mnemonic(seedChunk);
    // definir string para o mnemonic
    std::string mnemonic;
    // delimitador para a conversao de lista para string
    const char *const delim = " ";

    // definir ostring para o stream dos stringslist
    std::ostringstream os;
    // copiar os dados com o delimitador de espaco para ostring
    std::copy(writtenWord.begin(), writtenWord.end() - 1,
              std::ostream_iterator<std::string>(os, delim));
    // montar o resultado do ostring em nossa mnemonic
    mnemonic = os.str();
    std::cout
        << "\nmnemonic: " << mnemonic << std::endl;

    // definir nossa seed da entropia
    data_chunk seed = to_chunk(wallet::decode_mnemonic(split(mnemonic)));
    // criar a instancia de nossa chave
    bc::wallet::hd_private privateKey = bc::wallet::hd_private(seed, bc::wallet::hd_private::mainnet);
    // hd_key da chave
    wallet::hd_key keys = privateKey.to_hd_key();
    // pritar chave base16
    std::cout << "\nHex CHave Principal: " << std::endl;
    std::cout << encode_base16(keys) << std::endl;

    // criando nossa chave public comprimida
    wallet::hd_public publicKey = privateKey.to_public();

    std::cout << "Public Key: " << publicKey.encoded() << std::endl;

    std::cout << "\nEndereço gerado: " << std::endl;
    std::cout << wallet::ec_public(publicKey.point()).to_payment_address().encoded() << "\n"
              << std::endl;
}
// g++ -std=c++11 -o hd main.cpp $(pkg-config --cflags libbitcoin-system --libs libbitcoin-system) -lgmp