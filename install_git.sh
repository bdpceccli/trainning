#!/bin/bash


if [ $# == 0 ]
then
	echo "Usage $0 --username=mylogin --email=mymail@mydomain"
	exit -1
fi

while getopts u:e: flag
do
	case "${flag}" in
		u) username=${OPTARG};;
		e) email=${OPTARG};;
	esac
done

echo "Nombre param: $#"
echo "username=$username"
echo "email=$email"

if [ -x /usr/bin/git ]
then
	echo "git est déja installé : $(git --version)"
else
	apt-get install git
fi

# configuration de git
git config --global user.name $username
git config --global user.email $email
git config --global core.editor vim
git config --global init.defaultBranch main

echo "La configuration appliquée est :"
echo $(git config --list)

# configuration github
ssh-keygen -t ed25519 -C $email
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
echo "ajoutez le fichier ~/.ssh/id_ed25519.pub dans github"
echo "testez avec la commande: ssh -T git@github.com"

