#!/bin/bash

# Nome do programa
PROG_NAME="neotextus"

# Diretório temporário para a compilação
BUILD_DIR="/tmp/$PROG_NAME-build"

# Criação do diretório temporário
mkdir -p "$BUILD_DIR"

# Copia os arquivos do programa para o diretório temporário
cp -r * "$BUILD_DIR"

# Vai para o diretório temporário
cd "$BUILD_DIR" || exit 1

# Identifica a distribuição Linux
DISTRO=$(lsb_release -is)

# Instala as dependências necessárias de acordo com a distribuição
case "$DISTRO" in
    Ubuntu|Debian|Pop|Zorin)
        sudo apt-get update
        sudo apt-get install -y libncurses5-dev libncursesw5-dev
        ;;
    Fedora)
        sudo dnf install -y ncurses-devel
        ;;
    CentOS|RHEL)
        sudo yum install -y ncurses-devel
        ;;
    Arch)
        sudo pacman -Syu --noconfirm ncurses
        ;;
    *)
        echo "Distribuição não suportada: $DISTRO"
        exit 1
        ;;
esac

# Compila o programa usando o Makefile
make
sudo make install

# Limpa o diretório temporário
cd
rm -rf "$BUILD_DIR"

# Mensagem de sucesso
echo "Instalação completa! Você pode rodar o programa com o comando: $PROG_NAME"
