DefaultDir="$APP_DEFAULT_DIR/"
if [ "$(get_environment)" == "Graphical" ]; then
	for interface in kdialog Xdialog zenity; do
		if which $interface; then
			source "$DefaultDir"${interface}_interface.sh
			break
		else
			[ $interface == zenity ] && source $DefaultDir"text_interface.sh"
		fi
	done
else
	source "$DefaultDir"text_interface.sh
fi
