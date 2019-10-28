FROM ubuntu:trusty
ENV  PYTHONPATH=${PYTHONPATH}:/usr/lib/python3/dist-packages
RUN sudo apt-get update
RUN sudo apt-get install software-properties-common -y 
RUN sudo add-apt-repository ppa:ubuntu-sdk-team/ppa --yes && sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded --yes && sudo add-apt-repository ppa:ubuntu-toolchain-r/test --yes && sudo add-apt-repository ppa:beineri/opt-qt571-trusty -y &&  sudo apt-get update -qq 
RUN apt-get install curl git -y
RUN curl -L https://raw.githubusercontent.com/yyuu/pyenv-installer/master/bin/pyenv-installer | bash
ENV PATH=~/.pyenv/bin:${PATH}
RUN echo "eval \"$(pyenv init -)\"" >> ~/.bashrc
RUN echo "eval \"$(pyenv virtualenv-init -)\"" >> ~/.bashrc

RUN bash -c "pyenv uninstall -f 2.7.6" 
RUN sudo apt-get install -y gcc-9-base
RUN sudo apt-get install gcc-4.8 -y 
RUN sudo apt-get install fakeroot gcc g++ make -y 
RUN sudo apt-get install build-essential -y 
RUN sudo apt-get install -y libssl-dev libreadline-dev libbz2-dev 
RUN bash -c "pyenv install 3.5.4"
RUN bash -c "pyenv global 3.5.4"
#RUN pip install pillow

RUN apt-get --yes install libtiffxx5
RUN apt-get update
RUN apt-get --yes --force-yes install curl libmpfr4 libmpc3 libfox-1.6-dev libgtest-dev
RUN apt-get --yes --force-yes install gcc-arm-none-eabi
ENV QT_BASE=57
RUN apt-get update
RUN sudo apt-get install --yes --force-yes -qq qt${QT_BASE}base qt${QT_BASE}multimedia qt${QT_BASE}svg qt${QT_BASE}tools
RUN ls -alth /opt/qt${QT_BASE}/bin/qt${QT_BASE}-env.sh 
RUN echo "source /opt/qt${QT_BASE}/bin/qt${QT_BASE}-env.sh" >> ~/.bashrc
RUN echo "pyenv global 3.5.4" >> ~/.bashrc
RUN apt-get install -y cmake python-pip python-dev
RUN pip install pillow
RUN apt-get install -y libsdl-image1.2-dev 
