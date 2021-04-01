#include <bitcoin/system.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <list>
#include <iterator>

using namespace bc;

ec_compressed getPublicKey()
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
        << "mnemonic: " << mnemonic << std::endl;

    // definir nossa seed da entropia
    data_chunk seed = to_chunk(wallet::decode_mnemonic(split(mnemonic)));
    // criar a instancia de nossa chave
    bc::wallet::hd_private privateKey = bc::wallet::hd_private(seed, bc::wallet::hd_private::mainnet);
    // criando nossa chave public comprimida
    ec_compressed compressedPublicKey = privateKey.to_public().point();

    return compressedPublicKey;
}

bc::chain::witness::operation::list witness(ec_compressed publicKey)
{
    short_hash KeyHash = bitcoin_short_hash(publicKey);
    return {bc::chain::witness::operation(bc::machine::opcode(0)), bc::chain::witness::operation(to_chunk(KeyHash))};
}

int main()
{
    ec_compressed compressedPublicKey = getPublicKey();
    std::cout << "Chave pública Key: " << encode_base16(compressedPublicKey) << std::endl;
    bc::chain::script P2WPKH = bc::chain::script(witness(compressedPublicKey));
    std::cout << "Witness: " << P2WPKH.to_string(0) << std::endl;
    //Criando P2SH script
    short_hash WitnessProgramHash = bitcoin_short_hash(P2WPKH.to_data(0));
    bc::chain::script P2SH_P2WPKH = bc::chain::script::to_pay_script_hash_pattern(WitnessProgramHash);
    // mostrando p2sh script e endereço
    std::cout << "P2SH Script: " << P2SH_P2WPKH.to_string(0) << std::endl;
    std::cout << "Endereço gerado: " << bc::wallet::payment_address(P2WPKH, bc::wallet::payment_address::mainnet_p2sh).encoded() << std::endl;
}

// g++ -std=c++11 -o segwit.app main.cpp $(pkg-config --cflags libbitcoin-system --libs libbitcoin-system) -lgmp