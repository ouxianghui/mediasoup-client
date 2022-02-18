# mediasoup-client

# Mediasoup Server Depoly

1. Install Multipass 
    
    Multipass官网：[https://multipass.run/](https://multipass.run/)
    
2. Install Ubuntu 18.04LTS
    
    multipass launch -n media 18.04 -c 4 -m 4G -d 40G
    
3. Login ubuntu and install libraries
    
    1). add ssh key for git
    
    1. `ssh-keygen -t rsa -C "your_email@example.com"`
    2. copy the key of id_rsa.pub to github
    
    2). sudo update & upgrade
    
    3). sudo apt install build-essential
    
    4). install nvm
    
    1. cd ~
    2. install (from git or use the following cURL or Wget command)
        1. from git
            
            git clone [https://github.com/creationix/nvm.git](https://github.com/creationix/nvm.git) .nvm
            git checkout v0.39.1
            
        2. use the following cURL or Wget command
            
            `curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash`
            
            or
            
            `wget -qO- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash`
            
    3. update profile
        
        add the source lines from the snippet below to the correct profile file (`~/.bash_profile`, `~/.zshrc`, `~/.profile`, or `~/.bashrc`).
        
        `export NVM_DIR="$([ -z "${XDG_CONFIG_HOME-}" ] && printf %s "${HOME}/.nvm" || printf %s "${XDG_CONFIG_HOME}/nvm")"
        [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"` 
        
    4. install node
        
        nvm install v12.22.9
        
    5. install pip3
        
        sudo apt install python3-pip
        
    5). install mediasoup-demo
    
    1. git clone [https://github.com/versatica/mediasoup-demo.git](https://github.com/versatica/mediasoup-demo.git)
    2. cd mediasoup-demo/server
    3. npm install
    4. cp config.example.js config.js
    5. modify ip address in config.js
    6. npm start
    7. cd mediasoup-demo/app
    8. npm install 
    9. install gulp-cli: 
        1. npm install -g gulp-cli
    10. npm start
    11. [https://172.16.72.147:3000/?roomId=gvuxkens](https://172.16.72.147:3000/?roomId=gvuxkens)
