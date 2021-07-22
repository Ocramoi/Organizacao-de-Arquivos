#include "Join.h"

/* "SELECT * from Veiculos JOIN Linhas WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de veículos com [prefixo] dado com pesquisa na árvore [arqArvore] */
int selectJoinVeiculosLinhas(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha) {
    // Testa nome dos arquivo e arquivos abertos
    FILE *tabelaVeiculos = fopen(arqVeiculos, "rb"),
        *tabelaLinhas = fopen(arqLinhas, "rb");
    if (!arqVeiculos || !arqLinhas ||
        !tabelaVeiculos || !tabelaLinhas) {
        if (tabelaLinhas)
            fclose(tabelaLinhas);
        if (tabelaVeiculos)
            fclose(tabelaVeiculos);
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

/* "SELECT * from Veiculos JOIN Linhas WHERE INDEX ..." -> Seleciona e exibe veículos de [arqVeiculos] e linhas de [arqLinhas] com mesmo código de linha, realizando a pesquisa na árvore b */
int selectJoinVeiculosLinhasArvB(char *arqVeiculos, char *arqLinhas,
                                 char *campoVeiculos, char *campoLinha,
                                 char *indiceLinha) {
    // Testa nome dos arquivo e arquivos abertos
    FILE *tabelaVeiculos = fopen(arqVeiculos, "rb"),
        *tabelaLinhas = fopen(arqLinhas, "rb");
    if (!arqVeiculos || !arqLinhas || !indiceLinha ||
        !tabelaVeiculos || !tabelaLinhas) {
        if (tabelaVeiculos)
            fclose(tabelaVeiculos);
        if (tabelaLinhas)
            fclose(tabelaLinhas);
        return -1;
    }

    // Lê cabeçalhos de arquivos
    CABECALHO_VEICULOS_t *cabecalhoVeiculos = leCabecalhoVeiculos(tabelaVeiculos);
    CABECALHO_LINHAS_t *cabecalhoLinhas = leCabecalhoLinhas(tabelaLinhas);

    // Confere leitura de cabeçalhos
    if (!cabecalhoVeiculos || !cabecalhoLinhas) {
        fclose(tabelaVeiculos);
        fclose(tabelaLinhas);
        return -1;
    }

    // Confere status dos arquivos
    if (cabecalhoVeiculos->status != '1' || cabecalhoLinhas->status != '1') {
        fclose(tabelaVeiculos); fclose(tabelaLinhas);
        destroiCabecalhoVeiculos(cabecalhoVeiculos);
        destroiCabecalhoLinhas(cabecalhoLinhas);
        return -1;
    }

    // Confere número de registros
    if (cabecalhoVeiculos->nroRegs == 0 || cabecalhoLinhas->nroRegs == 0) {
        fclose(tabelaVeiculos); fclose(tabelaLinhas);
        destroiCabecalhoVeiculos(cabecalhoVeiculos);
        destroiCabecalhoLinhas(cabecalhoLinhas);
        return -1;
    }

    fclose(tabelaLinhas);
    destroiCabecalhoLinhas(cabecalhoLinhas);

    ARVB_t *arvLinhas = populaArvB(indiceLinha);
    if (!arvLinhas) {
        fclose(tabelaVeiculos);
        destroiCabecalhoVeiculos(cabecalhoVeiculos);
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

        int64_t offsetAchado = pesquisaArvB(arvLinhas, veiculo->codLinha);

        if (offsetAchado < 0) {
            destroiVeiculo(veiculo);
            continue;
        }

        exibido = 1;
        exibeDescreveVeiculo(cabecalhoVeiculos, veiculo);
        exibeLinhaOffset(arqLinhas, offsetAchado);
        printf("\n");

        // Libera memória de veículo atual
        destroiVeiculo(veiculo);
    }

    free(arvLinhas);

    // Fecha arquivos em uso
    fclose(tabelaVeiculos);
    // Libera memória alocada
    destroiCabecalhoVeiculos(cabecalhoVeiculos);

    return !exibido;
}

/* Realiza a ordenação e merge dos registros de veículos em [arqVeiculos] e linhas em [arqLinhas] com base no código de linha */
int ordMerge(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha) {
    int aux = ordenaVeiculos(arqVeiculos, arqVeiculos, "codLinha");
    aux ^= ordenaLinhas(arqLinhas, arqLinhas, "codLinha");
    if (aux)
        return -1;

    FILE *tabelaVeiculos = fopen(arqVeiculos, "rb"),
        *tabelaLinhas = fopen(arqLinhas, "rb");

    CABECALHO_VEICULOS_t *cabecalhoVeiculos = leCabecalhoVeiculos(tabelaVeiculos);
    CABECALHO_LINHAS_t *cabecalhoLinhas = leCabecalhoLinhas(tabelaLinhas);

    int idxVeiculos = 0,
        idxLinhas = 0;

    VEICULO_t *veiculo = leVeiculo(tabelaVeiculos);
    LINHA_t *linha = leObjLinha(tabelaLinhas);

    char exibido = 0;
    while (idxVeiculos < cabecalhoVeiculos->nroRegs &&
           idxLinhas < cabecalhoLinhas->nroRegs) {
        if (veiculo->codLinha == linha->codLinha) {
            exibido = 1;
            exibeDescreveVeiculo(cabecalhoVeiculos, veiculo);
            exibeDescreveLinha(cabecalhoLinhas, linha);
            printf("\n");

            idxVeiculos++;

            destroiVeiculo(veiculo);
            veiculo = NULL;
            if (idxVeiculos < cabecalhoVeiculos->nroRegs)
                veiculo = leVeiculo(tabelaVeiculos);
            continue;
        }

        if (veiculo->codLinha > linha->codLinha) {
            idxLinhas++;
            destroiLinha(linha);
            linha = NULL;
            if (idxLinhas < cabecalhoLinhas->nroRegs)
                linha = leObjLinha(tabelaLinhas);
            continue;
        }

        if (veiculo->codLinha < linha->codLinha) {
            idxVeiculos++;
            destroiVeiculo(veiculo);
            veiculo = NULL;
            if (idxVeiculos < cabecalhoVeiculos->nroRegs)
                veiculo = leVeiculo(tabelaVeiculos);
            continue;
        }
    }

    if (linha)
        destroiLinha(linha);
    if (veiculo)
        destroiVeiculo(veiculo);

    destroiCabecalhoLinhas(cabecalhoLinhas); destroiCabecalhoVeiculos(cabecalhoVeiculos);
    fclose(tabelaLinhas); fclose(tabelaVeiculos);

    return !exibido;
}
