#include "Join.h"

/* "SELECT * from Veiculos JOIN Linhas WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de veículos com [prefixo] dado com pesquisa na árvore [arqArvore] */
int selectJoinVeiculosLinhas(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha) {
    // Testa nome dos arquivo e arquivos abertos
    FILE *tabelaVeiculos = fopen(arqVeiculos, "rb"),
        *tabelaLinhas = fopen(arqLinhas, "rb");
    if (!arqVeiculos || !arqLinhas ||
        !tabelaVeiculos || !tabelaLinhas) {
        fclose(tabelaLinhas); fclose(tabelaVeiculos);
        return -1;
    }

    // Lê cabeçalhos de arquivos
    CABECALHO_VEICULOS_t *cabecalhoVeiculos = leCabecalhoVeiculos(tabelaVeiculos);
    CABECALHO_LINHAS_t *cabecalhoLinhas = leCabecalhoLinhas(tabelaLinhas);

    // Confere leitura de cabeçalhos
    if (!cabecalhoLinhas || !cabecalhoVeiculos) {
        fclose(tabelaVeiculos); fclose(tabelaLinhas);
        destroiCabecalhoVeiculos(cabecalhoVeiculos); destroiCabecalhoLinhas(cabecalhoLinhas);
        return -1;
    }

    // Confere status dos arquivos
    if (cabecalhoVeiculos->status != '1' || cabecalhoLinhas->status != '1') {
        fclose(tabelaVeiculos); fclose(tabelaLinhas);
        destroiCabecalhoVeiculos(cabecalhoVeiculos); destroiCabecalhoLinhas(cabecalhoLinhas);
        return -1;
    }

    // Confere número de registros
    if (cabecalhoVeiculos->nroRegs == 0 || cabecalhoLinhas->nroRegs == 0) {
        fclose(tabelaVeiculos); fclose(tabelaLinhas);
        destroiCabecalhoVeiculos(cabecalhoVeiculos); destroiCabecalhoLinhas(cabecalhoLinhas);
        return -1;
    }

    // Auxiliar para registro encontrado
    char exibido = 0;

    // Para cada veículo registrado
    for (int i = 0;
         i < cabecalhoVeiculos->nroRegs + cabecalhoVeiculos->nroRems;
         ++i) {
        // Lê novo objeto de veículo
        VEICULO_t *veiculo = leVeiculo(tabelaVeiculos);
        if (!veiculo)
            continue;

        // Retorna para o início de registros de linha
        fseek(tabelaLinhas, 82, SEEK_SET);
        for (int j = 0;
             j < cabecalhoLinhas->nroRegs + cabecalhoLinhas->nroRems;
             ++j) {
            // Lê novo objeto de linha
            LINHA_t *linha = leObjLinha(tabelaLinhas);
            if (!linha)
                continue;

            // Caso códigos batam, exibe corretamente
            if (veiculo->codLinha == linha->codLinha) {
                exibido = 1;
                exibeDescreveVeiculo(cabecalhoVeiculos, veiculo);
                exibeDescreveLinha(cabecalhoLinhas, linha);
                printf("\n");
            }

            // Libera memória de linha atual
            destroiLinha(linha);
        }

        // Libera memória de veículo atual
        destroiVeiculo(veiculo);
    }

    // Fecha arquivos em uso
    fclose(tabelaVeiculos); fclose(tabelaLinhas);
    // Libera memória alocada
    destroiCabecalhoVeiculos(cabecalhoVeiculos); destroiCabecalhoLinhas(cabecalhoLinhas);

    return !exibido;
}
