from setuptools import setup, find_packages

setup(
    name='spc',
    packages=find_packages(),
    version='0.1.0',
    install_requires=[          # 添加了依赖的 package
        'numpy>=1.18.2; python_version > "3.5"',
        'numpy<=1.16.6; python_version <= "3.5"',
    ]
)