# Blueprints for initc 0.5 - 0.9




## Program / environment controls

  **--name**		Rename the image to ... (restricted to available env space)

  **-E	_ENVVAR1,...._**	Kill all environment variables except for the named ones



## Running multiple commands in the container

 - Multiple services started in "background"
 - Last command becomes "main" (foreground / controls the container's lifecycle) 


   **initc** [*opts*] **--** `program-plus-args % program-plus-args % program-plus-args %%`


## Advanced synchronization primitives

   **-k**	/path/to/gatekeeper	Use program as a "gatekeeper"

   **-m**	NAME			Set "main" service (equivalent to "-c" mode)



## Service configuration

   **-C**	Path to config *directory* (default '`/etc/initc/`')
   - Main config file: initc.conf
   - A `service.d` subdirectory would contain service configuration files
	(syntax yet to be defined), read in lexicographical order
