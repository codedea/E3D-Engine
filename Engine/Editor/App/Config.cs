﻿using E3DEditor.Common;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Xml;

namespace E3DEditor
{
    public class Config
    {
        public static string GameResourcePath = "../";
        public static string GameTableFilePath = "";
        public static string JsonTablePath = "";
        public static string DialogAtlasPath = "";
        public static int GameWidth = 0;
        public static int GameHeight = 0;
        
        static XmlDocument xDoc = new XmlDocument();

        public static void Init()
        {
            xDoc = new XmlDocument();
            //readConfig();
        }

        private static void readConfig()
        {
            GameResourcePath = getAppConfig(CONST_STRING.Config_resourceFilePath);
            GameTableFilePath = getAppConfig(CONST_STRING.Config_tableFilePath);
            DialogAtlasPath = getAppConfig(CONST_STRING.Config_dialogAtlasFilePath);
            try
            {
                GameHeight = Convert.ToInt32(getAppConfig(CONST_STRING.Config_gameHeight));
                GameWidth = Convert.ToInt32(getAppConfig(CONST_STRING.Config_gameWidht));
            }
            catch
            {
                App.Instance.Log(LogLevel.Error, App.Instance["ResolutionRatioWrong"]);
            }
            JsonTablePath = getAppConfig(CONST_STRING.Config_jsonTablePath);
        }

        /// <summary>
        /// 写入配置
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        public static void WriteConfig(string key, string value)
        {
            if (value == "")
            {
                return;
            }
            setAppConfig(key, value);
        }

        /// <summary>
        /// 保存配置
        /// </summary>
        public static void SaveConfig()
        {
            xDoc.Save(System.Windows.Forms.Application.ExecutablePath + ".config");
            readConfig();
        }

        private static void setAppConfig(string appKey, string appValue)
        {
            var xNode = xDoc.SelectSingleNode("//appSettings");

            var xElem = (XmlElement)xNode.SelectSingleNode("//add[@key='" + appKey + "']");
            if (xElem != null) xElem.SetAttribute("value", appValue);
            else
            {
                var xNewElem = xDoc.CreateElement("add");
                xNewElem.SetAttribute("key", appKey);
                xNewElem.SetAttribute("value", appValue);
                xNode.AppendChild(xNewElem);
            }
        }

        private static string getAppConfig(string appKey)
        {
            xDoc.Load(System.Windows.Forms.Application.ExecutablePath + ".config");

            var xNode = xDoc.SelectSingleNode("//appSettings");

            var xElem = (XmlElement)xNode.SelectSingleNode("//add[@key='" + appKey + "']");

            if (xElem != null)
            {
                return xElem.Attributes["value"].Value;
            }
            return string.Empty;
        }
    }
}